#include "pch.h"
#include "Pawn.h"
#include "MovementComponent.h"

namespace bdt
{
    Pawn::Pawn(const String& tag) : Super(tag) { }

    Pawn::~Pawn() = default;

    void Pawn::Update(float deltaTime)
    {
        Super::Update(deltaTime);
        if (m_movementComponent) 
            m_movementComponent->Update(deltaTime);
    }

    MovementComponent* Pawn::GetMovementComponent() const
    {
        return m_movementComponent.get();
    }
}