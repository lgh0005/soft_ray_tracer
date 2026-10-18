#include "pch.h"
#include "InputManager.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace bdt
{
    InputManager::InputManager() = default;
    InputManager::~InputManager() = default;

    bool InputManager::Init(GLFWwindow* window)
    {
        if (!window) return false;
        if (m_keyboard || m_mouse) return false;

        m_keyboard = std::make_unique<Keyboard>(window);
        m_mouse = std::make_unique<Mouse>(window);

        return true;
    }

    void InputManager::Update()
    {
        if (m_keyboard) m_keyboard->Update();
        if (m_mouse)  m_mouse->Update();
    }

    void InputManager::Clear()
    {
        if (m_keyboard) m_keyboard->Clear();
        if (m_mouse) m_mouse->Clear();
    }

    void InputManager::Shutdown()
    {
        m_keyboard.reset();
        m_mouse.reset();
    }

    void InputManager::HandleScroll(float delta)
    {
        if (m_mouse) m_mouse->HandleScroll(delta);
    }
}