#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in ivec4 aBoneIDs;
layout(location = 5) in vec4 aWeights;

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

struct SkinnedInstanceProperty
{
    mat4 worldMatrix;
    uint objectID;
    uint boneOffset;
    uint boneCount;
    uint pad0;
};

layout(std430, binding = 0) readonly buffer InstanceBuffer
{
    SkinnedInstanceProperty instances[];
};

layout(std430, binding = 1) readonly buffer BoneBuffer
{
    mat4 boneMatrices[];
};

out VS_OUT
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec2 texCoord;
    vec3 worldTangent;
} vs_out;

void main()
{
    SkinnedInstanceProperty instance = instances[gl_InstanceID];
    mat4 skinMatrix = mat4(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        if (aBoneIDs[i] < 0 || aWeights[i] <= 0.0 || isnan(aWeights[i]) || isinf(aWeights[i])) continue;
        uint localID = uint(aBoneIDs[i]);
        uint paletteSize = uint(boneMatrices.length());
        if (localID >= instance.boneCount || instance.boneOffset >= paletteSize) continue;
        if (localID >= paletteSize - instance.boneOffset) continue;
        uint boneIndex = instance.boneOffset + localID;
        skinMatrix += boneMatrices[boneIndex] * aWeights[i];
        totalWeight += aWeights[i];
    }

    if (totalWeight > 0.0) skinMatrix /= totalWeight;
    else skinMatrix = mat4(1.0);

    vec4 skinnedPosition = skinMatrix * vec4(aPosition, 1.0);
    mat4 world = instance.worldMatrix;
    vec4 worldPosition = world * skinnedPosition;

    mat3 skinNormalMatrix = transpose(inverse(mat3(skinMatrix)));
    mat3 worldNormalMatrix = transpose(inverse(mat3(world)));
    mat3 normalMatrix = worldNormalMatrix * skinNormalMatrix;

    vs_out.worldPosition = worldPosition.xyz;
    vs_out.worldNormal = normalize(normalMatrix * aNormal);
    vs_out.worldTangent = normalize(mat3(world * skinMatrix) * aTangent);
    vs_out.texCoord = aTexCoord;

    gl_Position = viewProjection * worldPosition;
}
