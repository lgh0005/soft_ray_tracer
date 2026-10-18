#include "pch.h"
#include "Keyboard.h"

namespace bdt
{
    Keyboard::Keyboard(GLFWwindow* window) : Super(window) {}
    Keyboard::~Keyboard() = default;

    void Keyboard::Update()
    {
        if (!m_window) return;

        for (int32 key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            EInputState& state = m_states[key];

            if (glfwGetKey(m_window, key) == GLFW_PRESS)
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
    }

    void Keyboard::Clear()
    {
        m_states.fill(EInputState::None);
    }

    EInputState Keyboard::GetKeyState(int32 key) const
    {
        if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST) return EInputState::None;
        return m_states[key];
    }

    bool Keyboard::GetKeyDown(int32 key) const
    {
        return GetKeyState(key) == EInputState::Down;
    }

    bool Keyboard::GetKeyPress(int32 key) const
    {
        return GetKeyState(key) == EInputState::Press;
    }

    bool Keyboard::GetKeyUp(int32 key) const
    {
        return GetKeyState(key) == EInputState::Up;
    }

    bool Keyboard::GetKeyHold(int32 key) const
    {
        return GetKeyDown(key) || GetKeyPress(key);
    }
}
