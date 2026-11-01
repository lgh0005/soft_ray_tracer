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
        virtual LightData GetLightData() const override;
		virtual mat4 GetViewProjectionMatrix() const override;

        vec3 GetAttenuation() const;
        void SetRange(float range);
        float GetRange() const;

    private:
        float m_range{ 10.0f };
	};
}
