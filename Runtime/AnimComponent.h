#pragma once
#include "SceneComponent.h"
#include "AnimController.h"

namespace bdt
{
	class AnimComponent : public SceneComponent
	{
		using Super = SceneComponent;

	public:
		explicit AnimComponent(Actor* owner);
		~AnimComponent() override;

	public:
		void Update(float deltaTime);
		AnimController& GetController();
		const AnimController& GetController() const;
		bool EvaluatePose(const Model& model, List<mat4>& outputGlobalTransforms) const;

	private:
		AnimController m_controller;
	};
}

