#pragma once
#include "SceneComponent.h"

namespace bdt
{
	BDT_CLASS_PTR(Actor)
	BDT_CLASS_PTR(RootComponent)

	class MovementComponent : public SceneComponent
	{
		using Super = SceneComponent;

	public:
		MovementComponent(Actor* owner, RootComponent* root);
		virtual ~MovementComponent() override;

	public:
		virtual void Move(const vec3& direction, float deltaTime) = 0;
		virtual void Update(float deltaTime) = 0;

	protected:
		RootComponent* m_root{ nullptr };
	};
}
