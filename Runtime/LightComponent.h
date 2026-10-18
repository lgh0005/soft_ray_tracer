#pragma once
#include "RootComponent.h"
#include <Rendering/Uniforms.h>

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

        virtual LightData GetLightData() const = 0;

    protected:
        static vec3 GetAttenuationCoeff(float distance);

        vec3 m_color{ 1.0f };
        float m_intensity{ 1.0f };
    };
}

