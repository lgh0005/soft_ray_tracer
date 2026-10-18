#include "pch.h"
#include "CameraComponent.h"

namespace bdt
{
    CameraComponent::CameraComponent(Actor* owner) : Super(owner) {}
    CameraComponent::~CameraComponent() = default;

    float CameraComponent::GetFOV() const
    {
        return m_fov;
    }

    float CameraComponent::GetNearPlane() const
    {
        return m_nearPlane;
    }

    float CameraComponent::GetFarPlane() const
    {
        return m_farPlane;
    }

    const RenderViewport& CameraComponent::GetViewport() const
    {
        return m_viewport;
    }

    void CameraComponent::SetFOV(float fov)
    {
        m_fov = std::clamp(fov, 1.0f, 179.0f);
    }

    void CameraComponent::SetNearPlane(float nearPlane)
    {
        m_nearPlane = std::clamp(nearPlane, 0.001f, m_farPlane - 0.001f);
    }

    void CameraComponent::SetFarPlane(float farPlane)
    {
        m_farPlane = std::max(farPlane, m_nearPlane + 0.001f);
    }

    void CameraComponent::SetViewportSize(uint32 width, uint32 height)
    {
        m_viewport.Resize(width, height);
    }

    mat4 CameraComponent::GetViewMatrix() const
    {
        // 카메라의 World Transform을 반전하여 View Matrix 생성
        const mat4 rotation = glm::mat4_cast(glm::normalize(GetRotation()));
        const mat4 translation = glm::translate(mat4(1.0f), GetPosition());
        return glm::inverse(translation * rotation);
    }

    mat4 CameraComponent::GetProjectionMatrix() const
    {
        return glm::perspective
        (
            glm::radians(m_fov),
            m_viewport.GetAspectRatio(),
            m_nearPlane,
            m_farPlane
        );
    }

    CameraData CameraComponent::GetCameraData() const
    {
        CameraData data{};
        data.view = GetViewMatrix();
        data.projection = GetProjectionMatrix();
        data.viewProjection = data.projection * data.view;
        data.position = vec4(GetPosition(), 1.0f);
        data.nearPlane = m_nearPlane;
        data.farPlane = m_farPlane;
        data.viewportSize = vec2((float)m_viewport.GetWidth(), (float)m_viewport.GetHeight());

        return data;
    }
}
