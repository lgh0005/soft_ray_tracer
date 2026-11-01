#include "pch.h"
#include "PointLight.h"
#include "Enums.h"

namespace bdt
{
    PointLight::PointLight(Actor* owner) : Super(owner) { }

    PointLight::~PointLight() = default;

    float PointLight::GetRange() const
    {
        return m_range;
    }

    vec3 PointLight::GetAttenuation() const
    {
        return GetAttenuationCoeff(m_range);
    }

    void PointLight::SetRange(float range)
    {
        m_range = std::max(range, 0.0f);
    }

    LightData PointLight::GetLightData() const
    {
        LightData data{};

        // 광원 유형
        data.type = static_cast<uint32>(ELightType::Point);

        // 월드 공간 위치
        data.position = vec4(GetPosition(), 1.0f);

        // 공통 조명 속성
        data.color = vec4(m_color, 0.0f);
        data.intensity = m_intensity;

        // 거리 및 감쇠 계수
        data.range = m_range;

        const vec3 attenuation = GetAttenuation();
        data.attenuationConstant = attenuation.x;
        data.attenuationLinear = attenuation.y;
        data.attenuationQuadratic = attenuation.z;

        return data;
    }

    mat4 PointLight::GetViewProjectionMatrix() const
    {
        return mat4(1.0f);
    }
}