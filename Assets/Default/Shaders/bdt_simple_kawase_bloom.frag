#version 460 core

in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D uSource;
uniform int uMode; // 0: prefilter + downsample, 1: downsample, 2: upsample
uniform float uThreshold;

vec3 Downsample(vec2 texel)
{
    vec3 color = texture(uSource, vTexCoord).rgb * 0.5;
    color += texture(uSource, vTexCoord + vec2(-2.0, -2.0) * texel).rgb * 0.125;
    color += texture(uSource, vTexCoord + vec2( 2.0, -2.0) * texel).rgb * 0.125;
    color += texture(uSource, vTexCoord + vec2(-2.0,  2.0) * texel).rgb * 0.125;
    color += texture(uSource, vTexCoord + vec2( 2.0,  2.0) * texel).rgb * 0.125;
    return max(color, vec3(0.0));
}

vec3 Upsample(vec2 texel)
{
    vec3 color = vec3(0.0);
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            float weight = (x == 0 ? 2.0 : 1.0) * (y == 0 ? 2.0 : 1.0);
            color += texture(uSource, vTexCoord + vec2(x, y) * texel).rgb * weight;
        }
    }
    return color / 16.0;
}

void main()
{
    vec2 texel = 1.0 / vec2(textureSize(uSource, 0));
    vec3 color = uMode == 2 ? Upsample(texel) : Downsample(texel);
    if (uMode == 0 && dot(color, vec3(0.2126, 0.7152, 0.0722)) <= uThreshold)
        color = vec3(0.0);
    FragColor = vec4(color, 1.0);
}
