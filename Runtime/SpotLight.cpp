#include "pch.h"
#include "SpotLight.h"
#include "Enums.h"

namespace bdt
{
    SpotLight::SpotLight(Actor* owner)
        : Super(owner) {}

    SpotLight::~SpotLight() = default;

    float SpotLight::GetRange() const
    {
        return m_range;
    }

    float SpotLight::GetInnerConeAngle() const
    {
        return m_innerConeAngle;
    }

    float SpotLight::GetOuterConeAngle() const
    {
        return m_outerConeAngle;
    }

    vec3 SpotLight::GetAttenuation() const
    {
        return GetAttenuationCoeff(m_range);
    }

    void SpotLight::SetRange(float range)
    {
        m_range = std::max(range, 0.0f);
    }

    void SpotLight::SetConeAngles(float innerAngle, float outerAngle)
    {
        m_innerConeAngle = std::clamp(innerAngle, 0.0f, 90.0f);
        m_outerConeAngle = std::clamp(outerAngle, m_innerConeAngle, 90.0f);
    }

    LightData SpotLight::GetLightData() const
    {
        LightData data{};

        // 광원 유형
        data.type = static_cast<uint32>(ELightType::Spot);

        // 월드 공간 위치
        data.position = vec4(GetPosition(), 1.0f);

        // 월드 공간 조명 방향 (-Y 기준)
        const vec3 direction = GetRotation() * vec3(0.0f, -1.0f, 0.0f);
        data.direction = vec4(glm::normalize(direction), 0.0f);

        // 공통 조명 속성
        data.color = vec4(m_color, 0.0f);
        data.intensity = m_intensity;

        // 거리 및 감쇠 계수
        data.range = m_range;

        const vec3 attenuation = GetAttenuation();
        data.attenuationConstant = attenuation.x;
        data.attenuationLinear = attenuation.y;
        data.attenuationQuadratic = attenuation.z;

        // 원뿔 반각
        data.innerConeAngle = glm::cos(glm::radians(m_innerConeAngle));
        data.outerConeAngle = glm::cos(glm::radians(m_outerConeAngle));

        return data;
    }
}