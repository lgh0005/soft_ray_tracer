#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

layout(std140, binding = 0) uniform CameraBuffer
{
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraPosition;
    float nearPlane;
    float farPlane;
    vec2 viewportSize;
};

struct StaticInstanceProperty
{
    mat4 worldMatrix;
    uint objectID;
    float pad0;
    float pad1;
    float pad2;
};

layout(std430, binding = 0) readonly buffer InstanceBuffer
{
    StaticInstanceProperty instances[];
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
    mat4 world = instances[gl_InstanceID].worldMatrix;
    vec4 worldPosition = world * vec4(aPosition, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(world)));

    vs_out.worldPosition = worldPosition.xyz;
    vs_out.worldNormal = normalize(normalMatrix * aNormal);
    vs_out.worldTangent = normalize(mat3(world) * aTangent);
    vs_out.texCoord = aTexCoord;

    gl_Position = viewProjection * worldPosition;
}
