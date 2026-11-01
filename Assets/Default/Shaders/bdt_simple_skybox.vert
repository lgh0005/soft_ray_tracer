#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec3 vDirection;

layout(std140, binding = 3) uniform ScreenSpaceData
{
    mat4 invProjection;
    mat4 invViewRotation;
    float intensity;
};

void main()
{
    vec4 viewPosition = invProjection * vec4(aPosition.xy, 1.0, 1.0);
    vDirection = mat3(invViewRotation) * viewPosition.xyz;
    gl_Position = vec4(aPosition.xy, 1.0, 1.0);
}
