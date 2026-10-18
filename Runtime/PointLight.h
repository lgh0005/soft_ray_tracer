#pragma once
#include "LightComponent.h"

namespace bdt
{
	class PointLight : public LightComponent
	{
		using Super = LightComponent;
	
    public:
        explicit PointLight(Actor* owner);
        virtual ~PointLight() override;

    public:
        LightData GetLightData() const override;
        float GetRange() const;
        vec3 GetAttenuation() const;
        void SetRange(float range);

    private:
        float m_range{ 10.0f };
	};
}
