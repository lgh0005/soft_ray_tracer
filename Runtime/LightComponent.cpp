#include "pch.h"
#include "LightComponent.h"

namespace bdt
{
    LightComponent::LightComponent(Actor* owner)
        : Super(owner)
    {}

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