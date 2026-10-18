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
        LightData GetLightData() const override;

        float GetRange() const;
        float GetInnerConeAngle() const;
        float GetOuterConeAngle() const;
        vec3 GetAttenuation() const;

        void SetRange(float range);
        void SetConeAngles(float innerAngle, float outerAngle);

    private:
        float m_range{ 10.0f };

        // Degree 단위
        float m_innerConeAngle{ 12.5f };
        float m_outerConeAngle{ 17.5f };
    };
}