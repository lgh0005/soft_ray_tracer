#include "pch.h"
#include "FloatingMovement.h"
#include "RootComponent.h"

namespace bdt
{
    FloatingMovement::FloatingMovement(Actor* owner, RootComponent* root)
        : Super(owner, root)
    {
        const vec3 forward = m_root->GetRotation() * vec3(0.0f, 0.0f, -1.0f);
        m_yaw = std::atan2(-forward.x, -forward.z);
        m_pitch = std::asin(std::clamp(forward.y, -1.0f, 1.0f));
    }

    FloatingMovement::~FloatingMovement() = default;

    void FloatingMovement::Update(float deltaTime)
    {
        // WASD 이동 입력
        vec3 direction(0.0f);
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_W)) direction.z -= 1.0f;
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_S)) direction.z += 1.0f;
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_A)) direction.x -= 1.0f;
        if (BDT_KEYBOARD->GetKeyHold(GLFW_KEY_D)) direction.x += 1.0f;

        // 마우스 우클릭을 누르는 동안 시점 회전
        if (BDT_MOUSE->GetButtonHold(EMouseButton::Right))
        {
            const vec2 mouseDelta = BDT_MOUSE->GetDelta();
            if (glm::dot(mouseDelta, mouseDelta) > 0.0f)
                AddRotation(-mouseDelta.x * m_sensitivity, -mouseDelta.y * m_sensitivity);
        }

        // 이동 처리
        Move(direction, deltaTime);
    }

    void FloatingMovement::Move(const vec3& direction, float deltaTime)
    {
        if (deltaTime <= 0.0f) return;
        if (glm::dot(direction, direction) <= Epsilon<float>) return;

        // 대각선 이동 시 속도 증가 방지
        const vec3 normalizedDirection = glm::normalize(direction);

        // 카메라의 시선 방향을 기준으로 이동
        const quat rotation = glm::normalize(m_root->GetRotation());
        const vec3 worldDirection = rotation * normalizedDirection;

        m_root->Translate(worldDirection * m_moveSpeed * deltaTime);
    }

    void FloatingMovement::AddRotation(float yawDelta, float pitchDelta)
    {
        m_yaw += yawDelta;
        m_pitch += pitchDelta;

        // Pitch 범위 제한
        const float pitchLimit = glm::radians(89.0f);
        m_pitch = std::clamp(m_pitch, -pitchLimit, pitchLimit);

        // Yaw: 월드 Y축 회전
        const quat yawRotation = glm::angleAxis(m_yaw, vec3(0.0f, 1.0f, 0.0f));

        // Pitch: 로컬 X축 회전
        const quat pitchRotation = glm::angleAxis(m_pitch, vec3(1.0f, 0.0f, 0.0f));

        m_root->SetRotation(yawRotation * pitchRotation);
    }
}
