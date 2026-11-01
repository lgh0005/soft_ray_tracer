#pragma once
#include "LightComponent.h"

namespace bdt
{
    class SpotLight : public LightComponent
    {
        using Super = LightComponent;

    public:
        explicit SpotLight(Actor* owner);
        virtual ~SpotLight() override;

    public:
        virtual LightData GetLightData() const override;
        virtual mat4 GetViewProjectionMatrix() const override;

    public:
        float GetRange() const;
        float GetInnerConeAngle() const;
        float GetOuterConeAngle() const;
        vec3 GetAttenuation() const;

        void SetRange(float range);
        void SetConeAngles(float innerAngle, float outerAngle);

    private:
        float m_range{ 10.0f };
        float m_innerConeAngle{ 12.5f };
        float m_outerConeAngle{ 17.5f };
    };
}