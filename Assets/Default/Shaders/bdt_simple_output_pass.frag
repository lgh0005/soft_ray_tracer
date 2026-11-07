
#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 vUV;

// GBuffer
layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedo;
layout(binding = 3) uniform sampler2D gMaterial;
layout(binding = 4) uniform sampler2D gEmission;
layout(binding = 5) uniform sampler2D gDepth;

// Shadow Texture Array
layout(binding = 6) uniform sampler2DArray uShadowMaps;

// IBL Textures
layout(binding = 7) uniform samplerCube uIrradiance;
layout(binding = 8) uniform samplerCube uPrefiltered;
layout(binding = 9) uniform sampler2D uBRDFLUT;

uniform float uIBLIntensity;

// Camera UBO
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

// Shadow SSBO
struct ShadowData
{
    mat4 lightViewProjection;
    uint lightIndex;
    uint shadowMapIndex;
    vec2 pad0;
};

layout(std430, binding = 3) readonly buffer ShadowBuffer
{
    ShadowData shadows[];
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
    return F0 + (1.0 - F0) * pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
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

// Shadow Mapping + 3x3 PCF
float ShadowVisibility
(
    uint lightIndex,
    LightData light,
    vec3 position,
    vec3 N,
    vec3 L
)
{
    // 현재 Directional / Spot Light만 그림자 지원
    if (light.shadowIndex < 0)
        return 1.0;

    if (light.type != LIGHT_DIRECTIONAL && light.type != LIGHT_SPOT)
        return 1.0;

    uint shadowIndex = uint(light.shadowIndex);

    if (shadowIndex >= uint(shadows.length()))
        return 1.0;

    ShadowData shadow = shadows[shadowIndex];

    // LightData와 ShadowData의 대응 관계 확인
    if (shadow.lightIndex != lightIndex)
        return 1.0;

    // Shadow Texture Array의 Layer
    uint layer = shadow.shadowMapIndex;

    ivec3 dimensions = textureSize(uShadowMaps, 0);

    if (dimensions.x <= 0 || dimensions.y <= 0)
        return 1.0;

    if (layer >= uint(dimensions.z))
        return 1.0;

    // World Space -> Light Clip Space
    vec4 clip = shadow.lightViewProjection * vec4(position, 1.0);

    if (clip.w <= 0.0)
        return 1.0;

    // Clip Space -> Shadow Texture UV/Depth
    vec3 projected = clip.xyz / clip.w * 0.5 + 0.5;

    // 광원의 투영 범위를 벗어난 경우
    if (any(lessThan(projected, vec3(0.0))) ||
        any(greaterThan(projected, vec3(1.0))))
    {
        return 1.0;
    }

    // PCF
    vec2 texelSize = 1.0 / vec2(dimensions.xy);
    float bias = max
    (
        0.0005 * (1.0 - max(dot(N, L), 0.0)),
        0.00005
    );

    float visibility = 0.0;
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            vec2 uv = projected.xy + vec2(x, y) * texelSize;

            float depth = texture(
                uShadowMaps,
                vec3(uv, float(layer))
            ).r;

            visibility += projected.z - bias <= depth ? 1.0 : 0.0;
        }
    }

    return visibility / 9.0;
}

// 광원 하나의 직접광 계산
vec3 CalculateLight
(
    uint lightIndex,
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

            float spotFactor = clamp(
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

    float shadow = ShadowVisibility
    (
        lightIndex,
        light,
        worldPosition,
        N,
        L
    );

    // 완전히 가려진 영역에도 직접광의 25%를 남긴다.
    const float shadowStrength = 0.75;
    shadow = mix(1.0, shadow, shadowStrength);

    return (diffuse + specular) * radiance * NdotL * shadow;
}

vec3 CalculateIBL
(
    vec3 N,
    vec3 V,
    vec3 albedo,
    float metallic,
    float roughness
)
{
    float NdotV = max(dot(N, V), 0.0);

    // Metallic에 따른 기본 반사율
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Roughness를 고려한 Fresnel 근사
    vec3 F = F0 +
        (max(vec3(1.0 - roughness), F0) - F0) *
        pow(1.0 - NdotV, 5.0);

    // Diffuse 비율
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    // 1. Diffuse IBL
    vec3 irradiance = texture(uIrradiance, N).rgb;
    vec3 diffuse = kD * albedo * irradiance / PI;

    // 2. Specular IBL
    vec3 R = reflect(-V, N);

    float maxLOD = float(textureQueryLevels(uPrefiltered) - 1);

    vec3 prefiltered = textureLod
    (
        uPrefiltered,
        R,
        roughness * maxLOD
    ).rgb;

    // 3. BRDF LUT
    vec2 brdf = texture
    (
        uBRDFLUT,
        vec2(NdotV, roughness)
    ).rg;

    vec3 specular = prefiltered * (F0 * brdf.x + brdf.y);

    return (diffuse + specular) * uIBLIntensity;
}

void main()
{
    vec4 positionData = texture(gPosition, vUV);

    // 기하가 없는 픽셀
    if (positionData.w < 0.5)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 worldPosition = positionData.xyz;
    vec3 N = normalize(texture(gNormal, vUV).xyz);
    vec3 V = normalize(cameraPosition.xyz - worldPosition);

    vec3 albedo = texture(gAlbedo, vUV).rgb;
    vec4 material = texture(gMaterial, vUV);

    float metallic = clamp(material.r, 0.0, 1.0);
    float roughness = clamp(material.g, 0.045, 1.0);

    vec3 lighting = vec3(0.0);

    // 1. Direct PBR Lighting
    for (uint i = 0u; i < uint(lights.length()); ++i)
    {
        lighting += CalculateLight
        (
            i,
            lights[i],
            worldPosition,
            N,
            V,
            albedo,
            metallic,
            roughness
        );
    }

    // 2. Image Based Lighting
    if (uIBLIntensity > 0.0)
    {
        lighting += CalculateIBL
        (
            N,
            V,
            albedo,
            metallic,
            roughness
        );
    }

    // 3. Emission
    lighting += texture(gEmission, vUV).rgb;

    // 4. Linear HDR
    FragColor = vec4(lighting, 1.0);
}
