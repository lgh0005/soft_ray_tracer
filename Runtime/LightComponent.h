#pragma once
#include "RootComponent.h"
#include <Rendering/Instances.h>

namespace bdt
{
    class LightComponent : public RootComponent
    {
        using Super = RootComponent;

    public:
        explicit LightComponent(Actor* owner);
        virtual ~LightComponent() override;

    public:
        const vec3& GetColor() const;
        float GetIntensity() const;

        void SetColor(const vec3& color);
        void SetIntensity(float intensity);

        // Shadow Projection
        float GetShadowNearPlane() const;
        float GetShadowFarPlane() const;
        bool SetShadowNearPlane(float nearPlane);
        bool SetShadowFarPlane(float farPlane);
        bool SetShadowClipPlanes(float nearPlane, float farPlane);

        bool IsShadowEnabled() const;
        void SetShadowEnabled(bool enabled);

        virtual LightData GetLightData() const = 0;
        virtual mat4 GetViewProjectionMatrix() const = 0;

    protected:
        static vec3 GetAttenuationCoeff(float distance);

        vec3 m_color{ 1.0f };
        float m_intensity{ 1.0f };
        uint32 m_lightIndex{ 0 };

        bool m_castShadow{ false };
        float m_shadowNearPlane{ 0.1f };
        float m_shadowFarPlane{ 100.0f };
    };
}

