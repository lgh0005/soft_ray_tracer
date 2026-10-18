#pragma once
#include "MovementComponent.h"

namespace bdt
{
    class CharacterMovement : public MovementComponent
    {
        using Super = MovementComponent;

    public:
        CharacterMovement(Actor* owner, RootComponent* root);
        virtual ~CharacterMovement() override;

    public:
        void Update(float deltaTime) override;
        void Move(const vec3& direction, float deltaTime) override;
        bool IsMoving() const;

    private:
        float m_moveSpeed{ 1.0f };

        // [TEMP] 이동 결과에 따라 상태 변환
        bool m_isMoving{ false };
    };
}