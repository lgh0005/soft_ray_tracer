#version 460 core

in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D uLDRTexture;

float Luminance(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

vec3 FXAA(vec2 uv)
{
    vec2 texelSize = 1.0 / vec2(textureSize(uLDRTexture, 0));

    vec3 rgbM  = texture(uLDRTexture, uv).rgb;
    vec3 rgbNW = texture(uLDRTexture, uv + texelSize * vec2(-1.0,  1.0)).rgb;
    vec3 rgbNE = texture(uLDRTexture, uv + texelSize * vec2( 1.0,  1.0)).rgb;
    vec3 rgbSW = texture(uLDRTexture, uv + texelSize * vec2(-1.0, -1.0)).rgb;
    vec3 rgbSE = texture(uLDRTexture, uv + texelSize * vec2( 1.0, -1.0)).rgb;

    float lumaM  = Luminance(rgbM);
    float lumaNW = Luminance(rgbNW);
    float lumaNE = Luminance(rgbNE);
    float lumaSW = Luminance(rgbSW);
    float lumaSE = Luminance(rgbSE);

    // Edge 방향 계산
    vec2 direction;

    direction.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    direction.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    const float FXAA_REDUCE_MIN = 1.0 / 128.0;
    const float FXAA_REDUCE_MUL = 1.0 / 8.0;
    const float FXAA_SPAN_MAX = 8.0;

    float reduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) *
        (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN
    );

    float inverseMin = 1.0 / (
        min(abs(direction.x), abs(direction.y)) + reduce
    );

    direction = clamp(
        direction * inverseMin,
        vec2(-FXAA_SPAN_MAX),
        vec2(FXAA_SPAN_MAX)
    ) * texelSize;

    // 경계 방향으로 추가 샘플링
    vec3 rgbA = 0.5 * (
        texture(uLDRTexture, uv + direction * (1.0 / 3.0 - 0.5)).rgb +
        texture(uLDRTexture, uv + direction * (2.0 / 3.0 - 0.5)).rgb
    );

    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(uLDRTexture, uv + direction * -0.5).rgb +
        texture(uLDRTexture, uv + direction *  0.5).rgb
    );

    float lumaMin = min(
        lumaM,
        min(min(lumaNW, lumaNE), min(lumaSW, lumaSE))
    );

    float lumaMax = max(
        lumaM,
        max(max(lumaNW, lumaNE), max(lumaSW, lumaSE))
    );

    float lumaB = Luminance(rgbB);

    if (lumaB < lumaMin || lumaB > lumaMax)
        return rgbA;

    return rgbB;
}

void main()
{
    FragColor = vec4(FXAA(vTexCoord), 1.0);
}
