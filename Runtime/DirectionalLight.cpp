#include "pch.h"
#include "DirectionalLight.h"
#include "Enums.h"

namespace bdt
{
    DirectionalLight::DirectionalLight(Actor* owner)
        : Super(owner) { }

    DirectionalLight::~DirectionalLight() = default;

    LightData DirectionalLight::GetLightData() const
    {
        LightData data{};

        // 광원 유형
        data.type = static_cast<uint32>(ELightType::Directional);

        // 월드 공간 조명 방향 (-Y 기준)
        const vec3 direction = GetRotation() * vec3(0.0f, -1.0f, 0.0f);
        data.direction = vec4(glm::normalize(direction), 0.0f);

        // 공통 조명 속성
        data.color = vec4(m_color, 0.0f);
        data.intensity = m_intensity;

        return data;
    }
}
