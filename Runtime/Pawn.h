#pragma once
#include "Actor.h"

namespace bdt
{
	BDT_CLASS_PTR(MovementComponent)

	class Pawn : public Actor
	{
		using Super = Actor;

	public:
		Pawn(const String& tag);
		virtual ~Pawn() override;

	public:
		virtual void Update(float deltaTime) override;
		MovementComponent* GetMovementComponent() const;

	protected:
		MovementComponentUPtr m_movementComponent { nullptr };
	};
}
