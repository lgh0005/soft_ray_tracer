#include "pch.h"
#include "RectAreaLight.h"
#include "Enums.h"

namespace bdt
{
    RectAreaLight::RectAreaLight(Actor* owner) : Super(owner) { }

    RectAreaLight::~RectAreaLight() = default;

    void RectAreaLight::SetSize(const vec2& size)
    {
        if (size.x <= 0.0f || size.y <= 0.0f) return;
        m_size = size;
    }

    const vec2& RectAreaLight::GetSize() const
    {
        return m_size;
    }

    void RectAreaLight::SetTwoSided(bool enabled)
    {
        m_twoSided = enabled;
    }

    bool RectAreaLight::IsTwoSided() const
    {
        return m_twoSided;
    }

    LightData RectAreaLight::GetLightData() const
    {
        LightData data{};

        // 광원 유형
        data.type = static_cast<uint32>(ELightType::RectArea);

        // 월드 공간 위치
        data.position = vec4(GetPosition(), 1.0f);

        // 월드 공간 조명 방향 (-Y 기준)
        const quat rotation = GetRotation();
        const vec3 direction = rotation * vec3(0.0f, -1.0f, 0.0f);
        data.direction = vec4(glm::normalize(direction), 0.0f);

        // 면광원의 가로/세로 방향
        const vec3 right = rotation * vec3(1.0f, 0.0f, 0.0f);
        const vec3 up = rotation * vec3(0.0f, 0.0f, 1.0f);
        data.areaRight = vec4(glm::normalize(right), 0.0f);
        data.areaUp = vec4(glm::normalize(up), 0.0f);

        // 공통 조명 속성
        data.color = vec4(m_color, 0.0f);
        data.intensity = m_intensity;

        // 면광원 크기
        data.areaSize = vec4(m_size, 0.0f, 0.0f);

        // 양면 발광 설정
        data.lightFlags = m_twoSided ? 1u : 0u;

        return data;
    }

    mat4 RectAreaLight::GetViewProjectionMatrix() const
    {
        // Ray Traced Area Light는 Shadow Map을 사용하지 않음
        return mat4(1.0f);
    }
}
