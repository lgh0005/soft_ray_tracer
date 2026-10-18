#include "pch.h"
#include "MovementComponent.h"
#include "RootComponent.h"

namespace bdt
{
	MovementComponent::MovementComponent(Actor* owner, RootComponent* root)
		: Super(owner), m_root(root) { }

	MovementComponent::~MovementComponent() = default;
}
