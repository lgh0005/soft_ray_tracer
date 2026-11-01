#include "pch.h"
#include "LightComponent.h"

namespace bdt
{
    LightComponent::LightComponent(Actor* owner) : Super(owner) { }

    LightComponent::~LightComponent() = default;

    const vec3& LightComponent::GetColor() const
    {
        return m_color;
    }

    float LightComponent::GetIntensity() const
    {
        return m_intensity;
    }

    void LightComponent::SetColor(const vec3& color)
    {
        m_color = glm::max(color, vec3(0.0f));
    }

    void LightComponent::SetIntensity(float intensity)
    {
        m_intensity = std::max(intensity, 0.0f);
    }

    float LightComponent::GetShadowNearPlane() const
    {
        return m_shadowNearPlane;
    }

    float LightComponent::GetShadowFarPlane() const
    {
        return m_shadowFarPlane;
    }

    bool LightComponent::SetShadowNearPlane(float nearPlane)
    {
        return SetShadowClipPlanes(nearPlane, m_shadowFarPlane);
    }

    bool LightComponent::SetShadowFarPlane(float farPlane)
    {
        return SetShadowClipPlanes(m_shadowNearPlane, farPlane);
    }

    bool LightComponent::SetShadowClipPlanes(float nearPlane, float farPlane)
    {
        if (nearPlane <= 0.0f || farPlane <= nearPlane) return false;
        m_shadowNearPlane = nearPlane;
        m_shadowFarPlane = farPlane;
        return true;
    }

    bool LightComponent::IsShadowEnabled() const
    {
        return m_castShadow;
    }

    void LightComponent::SetShadowEnabled(bool enabled)
    {
        m_castShadow = enabled;
    }

    //void LightComponent::GetLightViewMatrix(mat4& view, float backwardOffset) const
    //{
    //    const vec3 position = GetPosition();
    //    const vec3 direction = GetRotation() * vec3(0.0f, -1.0f, 0.0f);
    //    const vec3 forward = glm::normalize(direction);
    //    const vec3 up = glm::abs(forward.y) < 0.999f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
    //    const vec3 eye = position - forward * backwardOffset;
    //    view = glm::lookAt(eye, eye + forward, up);
    //}

    vec3 LightComponent::GetAttenuationCoeff(float distance)
    {
        if (distance <= 7.0f)   return vec3(1.0f, 0.7f, 1.8f);
        if (distance <= 13.0f)  return vec3(1.0f, 0.35f, 0.44f);
        if (distance <= 20.0f)  return vec3(1.0f, 0.22f, 0.20f);
        if (distance <= 32.0f)  return vec3(1.0f, 0.14f, 0.07f);
        if (distance <= 50.0f)  return vec3(1.0f, 0.09f, 0.032f);
        if (distance <= 65.0f)  return vec3(1.0f, 0.07f, 0.017f);
        if (distance <= 100.0f) return vec3(1.0f, 0.045f, 0.0075f);
        if (distance <= 160.0f) return vec3(1.0f, 0.027f, 0.0028f);
        if (distance <= 200.0f) return vec3(1.0f, 0.022f, 0.0019f);
        if (distance <= 325.0f) return vec3(1.0f, 0.014f, 0.0007f);
        if (distance <= 600.0f) return vec3(1.0f, 0.007f, 0.0002f);
        return vec3(1.0f, 0.0014f, 0.000007f);
    }
}