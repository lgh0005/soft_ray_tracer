
#version 460 core

in vec3 vDirection;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform samplerCube uSkybox;
layout(binding = 1) uniform sampler2D uPosition;

layout(std140, binding = 3) uniform ScreenSpaceData
{
    mat4 invProjection;
    mat4 invViewRotation;
    float intensity;
};

void main()
{
    // 기하가 존재하는 픽셀은 건너뛴다.
    ivec2 pixel = ivec2(gl_FragCoord.xy);
    if (texelFetch(uPosition, pixel, 0).w >= 0.5)
        discard;

    // Environment Cubemap 샘플링
    vec3 direction = normalize(vDirection);
    vec3 hdr = max(textureLod(uSkybox, direction, 0.0).rgb * intensity, vec3(0.0));

    FragColor = vec4(hdr, 1.0);
}
