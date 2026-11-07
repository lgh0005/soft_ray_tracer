#version 460 core
#extension GL_ARB_shader_viewport_layer_array : require

layout(location = 0) in vec3 aPosition;

struct StaticInstanceProperty
{
    mat4 worldMatrix;
    uint objectID;
    uint pad0;
    uint pad1;
    uint pad2;
};

layout(std430, binding = 0) readonly buffer InstanceBuffer
{
    StaticInstanceProperty instances[];
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

    StaticInstanceProperty instance = instances[entry.instanceIndex];
    ShadowData shadow = shadows[entry.shadowIndex];

    gl_Position = shadow.lightViewProjection * instance.worldMatrix * vec4(aPosition, 1.0);
    gl_Layer = int(shadow.shadowMapIndex);
}
