#version 460 core
in vec2 vUV;
layout(location = 0) out float FragAO;
layout(binding = 0) uniform sampler2D uPosition;
layout(binding = 1) uniform sampler2D uNormal;
layout(binding = 2) uniform sampler2D uNoise;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uSamples[64];
uniform float uRadius;
uniform float uBias;

void main()
{
    vec4 position = texture(uPosition, vUV);
    if (position.w < 0.5) { FragAO = 1.0; return; }
    vec3 p = (uView * vec4(position.xyz, 1.0)).xyz;
    vec3 N = normalize(mat3(uView) * texture(uNormal, vUV).xyz);
    vec2 scale = vec2(textureSize(uPosition, 0)) / 4.0;
    vec3 randomVector = texture(uNoise, vUV * scale).xyz;
    vec3 tangent = randomVector - N * dot(randomVector, N);
    if (dot(tangent, tangent) < 1e-8)
        tangent = cross(abs(N.y) < 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0), N);
    vec3 T = normalize(tangent);
    mat3 basis = mat3(T, cross(N, T), N);
    float occlusion = 0.0;
    for (int i = 0; i < 64; ++i)
    {
        vec3 samplePosition = p + basis * uSamples[i] * uRadius;
        vec4 clip = uProjection * vec4(samplePosition, 1.0);
        if (clip.w <= 0.0) continue;
        vec2 uv = clip.xy / clip.w * 0.5 + 0.5;
        if (any(lessThan(uv, vec2(0))) || any(greaterThan(uv, vec2(1)))) continue;
        vec4 neighbor = texture(uPosition, uv);
        if (neighbor.w < 0.5) continue;
        float depth = (uView * vec4(neighbor.xyz, 1.0)).z;
        float rangeWeight = smoothstep(0.0, 1.0, uRadius / max(abs(p.z - depth), 1e-6));
        if (depth >= samplePosition.z + uBias) occlusion += rangeWeight;
    }
    FragAO = pow(clamp(1.0 - occlusion / 64.0, 0.0, 1.0), 4.0);
}
