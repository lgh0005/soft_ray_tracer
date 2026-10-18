#pragma once
#include "MovementComponent.h"

namespace bdt
{
    class FloatingMovement : public MovementComponent
    {
        using Super = MovementComponent;

    public:
        FloatingMovement(Actor* owner, RootComponent* root);
        ~FloatingMovement() override;

    public:
        void Update(float deltaTime) override;
        void Move(const vec3& direction, float deltaTime) override;
        void AddRotation(float yawDelta, float pitchDelta);

    private:
        float m_moveSpeed   { 5.0f };
        float m_sensitivity { 0.002f };
        float m_yaw   { 0.0f };
        float m_pitch { 0.0f };
    };
}
