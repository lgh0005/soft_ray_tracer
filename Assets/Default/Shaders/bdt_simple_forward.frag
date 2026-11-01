#version 460 core

in VS_OUT
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec2 texCoord;
    vec3 worldTangent;
} fs_in;


// EMaterialTextureSlot과 동일한 슬롯
layout(binding = 0) uniform sampler2D uAlbedoTexture;
layout(binding = 1) uniform sampler2D uNormalTexture;
layout(binding = 2) uniform sampler2D uAOTexture;
layout(binding = 3) uniform sampler2D uRoughnessTexture;
layout(binding = 4) uniform sampler2D uEmissionTexture;
layout(binding = 6) uniform sampler2D uMetallicTexture;
layout(binding = 7) uniform sampler2D uSpecularTexture;

layout(std140, binding = 1) uniform MaterialBuffer
{
    vec4 albedoFactor;
    vec4 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;
    float emissionStrength;
    float heightScale;
    uint textureFlags;
    uint pad0;
    uint pad1;
    uint pad2;
};

const uint TEXTURE_ALBEDO    = 1u << 0u;
const uint TEXTURE_NORMAL    = 1u << 1u;
const uint TEXTURE_AO        = 1u << 2u;
const uint TEXTURE_ROUGHNESS = 1u << 3u;
const uint TEXTURE_EMISSION  = 1u << 4u;
const uint TEXTURE_METALLIC  = 1u << 6u;
const uint TEXTURE_SPECULAR  = 1u << 7u;
const float NORMAL_EPSILON = 1e-8;
const float NORMAL_EPSILON2 = 1e-12;

bool HasTexture(uint flag)
{
    return (textureFlags & flag) != 0u;
}

vec3 GetWorldNormal(vec3 N, vec2 uv)
{
    // 미분값은 분기 이전에 계산한다.
    vec3 dpdx = dFdx(fs_in.worldPosition);
    vec3 dpdy = dFdy(fs_in.worldPosition);
    vec2 duvdx = dFdx(uv);
    vec2 duvdy = dFdy(uv);

    if (!HasTexture(TEXTURE_NORMAL))
        return N;

    float determinant = duvdx.x * duvdy.y - duvdx.y * duvdy.x;
    if (abs(determinant) < NORMAL_EPSILON) return N;

    // 위치와 UV의 미분으로 접선 공간을 구성한다.
    vec3 tangent = (dpdx * duvdy.y - dpdy * duvdx.y) / determinant;
    vec3 bitangent = (-dpdx * duvdy.x + dpdy * duvdx.x) / determinant;
    tangent -= N * dot(N, tangent);
    if (dot(tangent, tangent) < NORMAL_EPSILON2)
        return N;

    vec3 T = normalize(tangent);

    // 미러 UV의 방향을 반영한다.
    float handedness = dot(cross(N, T), bitangent) < 0.0 ? -1.0 : 1.0;
    vec3 B = normalize(cross(N, T)) * handedness;

    // 현재 자산은 BC5 UNORM: RG에서 XY를 읽고 Z를 복원한다.
    vec2 xy = texture(uNormalTexture, uv).rg * 2.0 - 1.0;
    float z = sqrt(max(1.0 - dot(xy, xy), 0.0));
    vec3 tangentNormal = normalize(vec3(xy, z));

    return normalize(mat3(T, B, N) * tangentNormal);
}

layout(std140, binding = 0) uniform CameraBuffer
{
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inverseViewProjection;
    vec4 cameraPosition;

    float nearPlane;
    float farPlane;
    vec2 viewportSize;
};

// Light SSBO
struct LightData
{
    vec4 position;
    vec4 direction;
    vec4 color;

    float intensity;
    float range;
    float innerConeAngle;
    float outerConeAngle;

    uint type;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;

    int shadowIndex;
};

layout(std430, binding = 2) readonly buffer LightBuffer
{
    LightData lights[];
};

const float PI = 3.14159265359;
const float EPSILON = 0.0001;
const uint LIGHT_DIRECTIONAL = 0u;
const uint LIGHT_POINT = 1u;
const uint LIGHT_SPOT = 2u;

// GGX Normal Distribution
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float safeRoughness = clamp(roughness, 0.045, 1.0);
    float a = safeRoughness * safeRoughness;
    float a2 = a * a;

    float NdotH = clamp(dot(N, H), 0.0, 1.0);
    float NdotH2 = NdotH * NdotH;
    float numerator = a2;
    // 작은 roughness의 피크를 보존하고 a2 - 1의 상쇄 오차를 피한다.
    float denominator = (1.0 - NdotH2) + a2 * NdotH2;

    return numerator / (PI * denominator * denominator);
}

// Schlick-GGX Geometry
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
}

// Smith Geometry
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness)
         * GeometrySchlickGGX(NdotL, roughness);
}

// Schlick Fresnel
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) *  pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

// Point / Spot 거리 감쇠
float CalculateAttenuation(LightData light, float distance)
{
    if (distance >= light.range)
        return 0.0;

    float denominator =
        light.attenuationConstant +
        light.attenuationLinear * distance +
        light.attenuationQuadratic * distance * distance;

    float attenuation = 1.0 / max(denominator, EPSILON);

    float rangeFactor = clamp
    (
        1.0 - distance / max(light.range, EPSILON),
        0.0,
        1.0
    );

    return attenuation * rangeFactor * rangeFactor;
}

// 광원 하나의 직접광 계산
vec3 CalculateLight
(
    LightData light,
    vec3 worldPosition,
    vec3 N,
    vec3 V,
    vec3 albedo,
    float metallic,
    float roughness
)
{
    vec3 L;
    float attenuation = 1.0;

    if (light.type == LIGHT_DIRECTIONAL)
    {
        L = normalize(-light.direction.xyz);
    }
    else if (light.type == LIGHT_POINT || light.type == LIGHT_SPOT)
    {
        vec3 toLight = light.position.xyz - worldPosition;
        float distance = length(toLight);

        if (distance >= light.range)
            return vec3(0.0);

        L = toLight / max(distance, EPSILON);
        attenuation = CalculateAttenuation(light, distance);

        if (light.type == LIGHT_SPOT)
        {
            float theta = dot(-L, normalize(light.direction.xyz));
            float epsilon = light.innerConeAngle - light.outerConeAngle;
            float spotFactor = clamp
            (
                (theta - light.outerConeAngle) / max(epsilon, EPSILON),
                0.0,
                1.0
            );

            attenuation *= spotFactor;
        }
    }
    else
    {
        return vec3(0.0);
    }

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    if (NdotL <= 0.0 || NdotV <= 0.0)
        return vec3(0.0);

    vec3 H = normalize(V + L);

    // Metallic에 따른 기본 반사율
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = max(4.0 * NdotV * NdotL, EPSILON);
    vec3 specular = numerator / denominator;

    // 에너지 보존 근사
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    vec3 diffuse = kD * albedo / PI;
    vec3 radiance = light.color.rgb * light.intensity * attenuation;

    return (diffuse + specular) * radiance * NdotL;
}

layout(location = 0) out vec4 FragColor;
layout(binding = 8) uniform sampler2D uSSAO;
uniform bool uHasSSAO;
uniform uint uLightCount;
void main()
{
    vec2 uv = fs_in.texCoord;
    vec4 albedo = albedoFactor;
    if (HasTexture(TEXTURE_ALBEDO)) albedo *= texture(uAlbedoTexture, uv);
    vec3 N = GetWorldNormal(normalize(fs_in.worldNormal), uv);
    vec3 V = normalize(cameraPosition.xyz - fs_in.worldPosition);
    float roughness = roughnessFactor;
    if (HasTexture(TEXTURE_ROUGHNESS)) roughness *= texture(uRoughnessTexture, uv).r;
    roughness = clamp(roughness, 0.045, 1.0);
    float metallic = metallicFactor;
    if (HasTexture(TEXTURE_METALLIC)) metallic *= texture(uMetallicTexture, uv).r;
    metallic = clamp(metallic, 0.0, 1.0);
    if (HasTexture(TEXTURE_SPECULAR))
    {
        vec3 sourceF0 = clamp(texture(uSpecularTexture, uv).rgb, 0.0, 1.0);
        float specularStrength = max(max(sourceF0.r, sourceF0.g), sourceF0.b);
        vec3 fromDiffuse = albedo.rgb * (1.0 - specularStrength) / (0.96 * max(1.0 - metallic, 0.0001));
        vec3 fromSpecular = (sourceF0 - vec3(0.04 * (1.0 - metallic))) / max(metallic, 0.0001);
        albedo.rgb = clamp(mix(fromDiffuse, fromSpecular, metallic * metallic), 0.0, 1.0);
    }
    float ao = HasTexture(TEXTURE_AO) ? texture(uAOTexture, uv).r : 1.0;
    if (uHasSSAO) ao *= texture(uSSAO, gl_FragCoord.xy / viewportSize).r;
    vec3 color = vec3(0.03) * albedo.rgb * clamp(ao, 0.0, 1.0);
    for (uint i = 0u; i < min(uLightCount, uint(lights.length())); ++i)
        color += CalculateLight(lights[i], fs_in.worldPosition, N, V, albedo.rgb, metallic, roughness);
    vec3 emission = emissiveFactor.rgb * max(emissionStrength, 0.0);
    if (HasTexture(TEXTURE_EMISSION)) emission *= texture(uEmissionTexture, uv).rgb;
    FragColor = vec4(color + emission, albedo.a);
}
