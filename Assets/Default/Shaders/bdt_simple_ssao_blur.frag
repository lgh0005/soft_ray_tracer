#version 460 core
in vec2 vUV;
layout(location = 0) out float FragAO;
layout(binding = 0) uniform sampler2D uInput;
void main()
{
    ivec2 size = textureSize(uInput, 0);
    ivec2 center = ivec2(vUV * vec2(size));
    float sum = 0.0;
    for (int y = -2; y < 2; ++y)
        for (int x = -2; x < 2; ++x)
            sum += texelFetch(uInput, clamp(center + ivec2(x, y), ivec2(0), size - 1), 0).r;
    FragAO = sum / 16.0;
}
