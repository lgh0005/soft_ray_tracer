#include "pch.h"
#include "Mouse.h"

namespace bdt
{
    Mouse::Mouse(GLFWwindow* window) : IInputDevice(window) { }
    Mouse::~Mouse() = default;

    void Mouse::Update()
    {
        if (!m_window) return;

        // 1. 마우스 위치 갱신
        double x = 0.0;
        double y = 0.0;
        glfwGetCursorPos(m_window, &x, &y);

        vec2 currentPosition(static_cast<float>(x), static_cast<float>(y));
        if (m_hasPreviousPosition) m_delta = currentPosition - m_position;
        else
        {
            m_delta = vec2(0.0f);
            m_hasPreviousPosition = true;
        }

        m_position = currentPosition;

        // 2. 마우스 버튼 상태 갱신
        constexpr int32 buttons[static_cast<usize>(EMouseButton::Count)] =
        {
            GLFW_MOUSE_BUTTON_LEFT,
            GLFW_MOUSE_BUTTON_RIGHT,
            GLFW_MOUSE_BUTTON_MIDDLE
        };

        for (usize i = 0; i < static_cast<usize>(EMouseButton::Count); ++i)
        {
            EInputState& state = m_states[i];

            if (glfwGetMouseButton(m_window, buttons[i]) == GLFW_PRESS)
            {
                if (state == EInputState::Press || state == EInputState::Down) state = EInputState::Press;
                else state = EInputState::Down;
            }
            else
            {
                if (state == EInputState::Press || state == EInputState::Down) state = EInputState::Up;
                else state = EInputState::None;
            }
        }

        // 3. 마우스 스크롤 갱신
        m_wheelDelta = m_pendingWheelDelta;
        m_pendingWheelDelta = 0.0f;
    }

    void Mouse::Clear()
    {
        m_states.fill(EInputState::None);

        m_position = vec2(0.0f);
        m_delta = vec2(0.0f);

        m_wheelDelta = 0.0f;
        m_pendingWheelDelta = 0.0f;

        m_hasPreviousPosition = false;
    }

    EInputState Mouse::GetButtonState(EMouseButton button) const
    {
        usize index = static_cast<usize>(button);

        if (index >= static_cast<usize>(EMouseButton::Count))
            return EInputState::None;

        return m_states[index];
    }

    bool Mouse::GetButtonDown(EMouseButton button) const
    {
        return GetButtonState(button) == EInputState::Down;
    }

    bool Mouse::GetButtonPress(EMouseButton button) const
    {
        return GetButtonState(button) == EInputState::Press;
    }

    bool Mouse::GetButtonUp(EMouseButton button) const
    {
        return GetButtonState(button) == EInputState::Up;
    }

    bool Mouse::GetButtonHold(EMouseButton button) const
    {
        auto state = GetButtonState(button);
        return state == EInputState::Down || state == EInputState::Press;
    }

    void Mouse::HandleScroll(float delta)
    {
        m_pendingWheelDelta += delta;
    }
}