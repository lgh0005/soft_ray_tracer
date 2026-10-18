#pragma once
#include "LightComponent.h"

namespace bdt
{
	class DirectionalLight : public LightComponent
	{
		using Super = LightComponent;

	public:
		explicit DirectionalLight(Actor* owner);
		~DirectionalLight() override;

		LightData GetLightData() const override;
	};
}
