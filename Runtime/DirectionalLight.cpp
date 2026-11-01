#include "pch.h"
#include "DirectionalLight.h"
#include "Enums.h"

namespace bdt
{
    DirectionalLight::DirectionalLight(Actor* owner) : Super(owner) { }

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

    mat4 DirectionalLight::GetViewProjectionMatrix() const
    {
        // View Matrix
        const vec3 position = GetPosition();
        const vec3 forward = glm::normalize(GetRotation() * vec3(0.0f, -1.0f, 0.0f));
        vec3 up(0.0f, 1.0f, 0.0f);

        const vec3 absForward = glm::abs(forward);
        if (absForward.y > absForward.x && absForward.y > absForward.z)
            up = vec3(1.0f, 0.0f, 0.0f);

        const vec3 eye = position - forward * m_shadowBackwardOffset;
        const mat4 view = glm::lookAt(eye, eye + forward, up);

        // Orthographic Shadow Projection
        const vec2 halfSize = m_shadowOrthoSize * 0.5f;
        const mat4 projection = glm::ortho
        (
            -halfSize.x, halfSize.x,
            -halfSize.y, halfSize.y,
            m_shadowNearPlane,
            m_shadowFarPlane
        );

        return projection * view;
    }

    const vec2& DirectionalLight::GetShadowOrthoSize() const
    {
        return m_shadowOrthoSize;
    }

    void DirectionalLight::SetShadowOrthoSize(const vec2& size)
    {
        if (size.x <= 0.0f || size.y <= 0.0f) return;
        m_shadowOrthoSize = size;
    }

    float DirectionalLight::GetShadowBackwardOffset() const
    {
        return m_shadowBackwardOffset;
    }

    void DirectionalLight::SetShadowBackwardOffset(float offset)
    {
        if (offset < 0.0f) return;
        m_shadowBackwardOffset = offset;
    }
}
