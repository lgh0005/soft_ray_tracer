#version 460 core

in VS_OUT
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec2 texCoord;
    vec3 worldTangent;
} fs_in;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gMaterial;
layout(location = 4) out vec4 gEmission;

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
    float ior;
    float transmission;
    uint textureFlags;
    uint pad0;
    uint pad1;
};

const uint TEXTURE_ALBEDO    = 1u << 0u;
const uint TEXTURE_NORMAL    = 1u << 1u;
const uint TEXTURE_AO        = 1u << 2u;
const uint TEXTURE_ROUGHNESS = 1u << 3u;
const uint TEXTURE_EMISSION  = 1u << 4u;
const uint TEXTURE_METALLIC  = 1u << 6u;
const uint TEXTURE_SPECULAR  = 1u << 7u;
const float EPSILON = 1e-8;
const float EPSILON2 = 1e-12;

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
    if (abs(determinant) < EPSILON) return N;

    // 위치와 UV의 미분으로 접선 공간을 구성한다.
    vec3 tangent = (dpdx * duvdy.y - dpdy * duvdx.y) / determinant;
    vec3 bitangent = (-dpdx * duvdy.x + dpdy * duvdx.x) / determinant;
    tangent -= N * dot(N, tangent);
    if (dot(tangent, tangent) < EPSILON2)
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

void main()
{
    vec2 uv = fs_in.texCoord;

    // Albedo
    vec4 albedo = albedoFactor;

    // 앞선 테스트에서 *= 대신 =를 사용했다면 여기서도 유지한다.
    if (HasTexture(TEXTURE_ALBEDO))
        albedo *= texture(uAlbedoTexture, uv);

    // BC5 노멀맵 적용을 복원한다.
    vec3 normal = GetWorldNormal(normalize(fs_in.worldNormal), uv);

    // AO: 독립 텍스처의 R 채널
    float ao = 1.0;

    if (HasTexture(TEXTURE_AO))
        ao = texture(uAOTexture, uv).r;

    // Roughness: 독립 텍스처의 R 채널
    float roughness = roughnessFactor;

    if (HasTexture(TEXTURE_ROUGHNESS))
        roughness *= texture(uRoughnessTexture, uv).r;

    // Metallic: 독립 선형 텍스처의 R 채널
    float metallic = metallicFactor;
    if (HasTexture(TEXTURE_METALLIC))
        metallic *= texture(uMetallicTexture, uv).r;

    // 기존 Diffuse/Specular를 Metallic/Roughness의 Base Color로 근사 변환한다.
    // 금속 영역에 상수 색을 칠하지 않고 원본 F0의 색과 세기를 보존한다.
    if (HasTexture(TEXTURE_SPECULAR))
    {
        float m = clamp(metallic, 0.0, 1.0);
        vec3 sourceF0 = clamp(texture(uSpecularTexture, uv).rgb, 0.0, 1.0);
        float specularStrength = max(max(sourceF0.r, sourceF0.g), sourceF0.b);
        vec3 fromDiffuse = albedo.rgb * (1.0 - specularStrength) / (0.96 * max(1.0 - m, 0.0001));
        vec3 fromSpecular = (sourceF0 - vec3(0.04 * (1.0 - m))) / max(m, 0.0001);
        albedo.rgb = clamp(mix(fromDiffuse, fromSpecular, m * m), 0.0, 1.0);
    }

    gPosition = vec4(fs_in.worldPosition, 1.0);
    gNormal = vec4(normal, 0.0);
    gAlbedo = albedo;
    gMaterial = vec4
    (
        clamp(metallic, 0.0, 1.0),
        clamp(roughness, 0.045, 1.0),
        clamp(ao, 0.0, 1.0),
        0.0
    );

    // 선형 공간에서 발광 색상과 세기를 계산한다.
    vec3 emission = emissiveFactor.rgb * max(emissionStrength, 0.0);
    if (HasTexture(TEXTURE_EMISSION))
        emission *= texture(uEmissionTexture, uv).rgb;

    gEmission = vec4(emission, 0.0);

    // Height는 현재 패스에서 처리하지 않는다.
}
