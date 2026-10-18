#include "pch.h"
#include "CharacterMovement.h"
#include "RootComponent.h"

namespace bdt
{
    CharacterMovement::CharacterMovement(Actor* owner, RootComponent* root)
        : Super(owner, root) { }

    CharacterMovement::~CharacterMovement() = default;

    void CharacterMovement::Update(float deltaTime)
    {
        // [TEMP] 이번 프레임에 이동하지 않으면 Idle로 전환할 수 있도록 초기화한다.
        m_isMoving = false;

        // 방향키 이동 입력
        vec3 direction(0.0f);

        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_UP))    direction.z -= 1.0f;
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_DOWN))  direction.z += 1.0f;
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_LEFT))  direction.x -= 1.0f;
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_RIGHT)) direction.x += 1.0f;

        // 이동 처리
        Move(direction, deltaTime);
    }

    void CharacterMovement::Move(const vec3& direction, float deltaTime)
    {
        if (deltaTime <= 0.0f) return;
        if (glm::dot(direction, direction) <= Epsilon<float>) return;

        // 대각선 이동 시 속도 증가 방지
        const vec3 normalizedDirection = glm::normalize(direction);

        // 캐릭터의 현재 회전을 이동 방향에 적용
        const quat rotation = m_root->GetRotation();
        vec3 worldDirection = rotation * normalizedDirection;

        // 수평 이동만 허용
        worldDirection.y = 0.0f;
        if (glm::dot(worldDirection, worldDirection) <= Epsilon<float>)
            return;

        worldDirection = glm::normalize(worldDirection);

        m_root->Translate(worldDirection * m_moveSpeed * deltaTime);
        m_isMoving = true;
    }

    bool CharacterMovement::IsMoving() const
    {
        return m_isMoving;
    }
}
