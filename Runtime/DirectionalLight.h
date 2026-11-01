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

		virtual LightData GetLightData() const override;
		virtual mat4 GetViewProjectionMatrix() const override;

	public:
		const vec2& GetShadowOrthoSize() const;
		void SetShadowOrthoSize(const vec2& size);
		float GetShadowBackwardOffset() const;
		void SetShadowBackwardOffset(float offset);

	private:
		vec2 m_shadowOrthoSize{ 40.0f, 40.0f };
		float m_shadowBackwardOffset{ 40.0f };
	};
}
