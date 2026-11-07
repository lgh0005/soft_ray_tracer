#version 460 core
#extension GL_ARB_shader_viewport_layer_array : require

layout(location = 0) in vec3 aPosition;
layout(location = 4) in ivec4 aBoneIDs;
layout(location = 5) in vec4 aWeights;

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

// EInstanceBinding::ShadowProperty must equal 4.
struct ShadowInstanceProperty
{
    uint instanceIndex;
    uint shadowIndex;
    uvec2 pad0; // CPU alignas(16) 구조체와 배열 stride를 맞춘다.
};

layout(std430, binding = 4) readonly buffer ShadowInstanceBuffer
{
    ShadowInstanceProperty shadowInstances[];
};

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

void main()
{
    uint drawIndex = uint(gl_BaseInstance) + uint(gl_InstanceID);
    if (drawIndex >= uint(shadowInstances.length()))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        gl_Layer = 0;
        return;
    }

    ShadowInstanceProperty entry = shadowInstances[drawIndex];
    if (entry.instanceIndex >= uint(instances.length()) || entry.shadowIndex >= uint(shadows.length()))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        gl_Layer = 0;
        return;
    }

    SkinnedInstanceProperty instance = instances[entry.instanceIndex];
    ShadowData shadow = shadows[entry.shadowIndex];

    vec4 localPosition = vec4(aPosition, 1.0);
    vec4 skinnedPosition = vec4(0.0);
    float totalWeight = 0.0;
    uint paletteSize = uint(boneMatrices.length());

    for (int i = 0; i < 4; ++i)
    {
        int boneID = aBoneIDs[i];
        float weight = aWeights[i];
        if (boneID < 0 || weight <= 0.0 || isnan(weight) || isinf(weight)) continue;

        uint localBoneID = uint(boneID);
        if (localBoneID >= instance.boneCount || instance.boneOffset >= paletteSize) continue;
        if (localBoneID >= paletteSize - instance.boneOffset) continue;

        uint boneIndex = instance.boneOffset + localBoneID;
        skinnedPosition += (boneMatrices[boneIndex] * localPosition) * weight;
        totalWeight += weight;
    }

    if (totalWeight > 0.0) skinnedPosition /= totalWeight;
    else skinnedPosition = localPosition;

    gl_Position = shadow.lightViewProjection * instance.worldMatrix * skinnedPosition;
    gl_Layer = int(shadow.shadowMapIndex);
}
