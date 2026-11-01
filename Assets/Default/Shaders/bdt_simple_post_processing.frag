#version 460 core

in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D uSceneTexture;
layout(binding = 1) uniform sampler2D uBloomTexture;

uniform float uExposure;
uniform float uGamma;
uniform float uBloomStrength;

vec3 ACESFilm(vec3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp
    (
        (color * (a * color + b)) /
        (color * (c * color + d) + e),
        0.0, 1.0
    );
}

// HDR -> Exposure -> ACES -> Gamma
vec3 SampleColor(vec2 uv)
{
    vec3 hdr = texture(uSceneTexture, uv).rgb;
    if (uBloomStrength > 0.0)
        hdr += texture(uBloomTexture, uv).rgb * uBloomStrength;
    hdr *= uExposure;
    vec3 ldr = ACESFilm(max(hdr, vec3(0.0)));
    return pow(ldr, vec3(1.0 / max(uGamma, 0.001)));
}

void main()
{
    vec3 color = SampleColor(vTexCoord);
    FragColor = vec4(color, 1.0);
}
