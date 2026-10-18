#include "pch.h"
#include "WindowManager.h"
#include "GLWindow.h"
#include "GLMainContext.h"
#include "Logger.h"

namespace bdt
{
	WindowManager::WindowManager() = default;
	WindowManager::~WindowManager() = default;

    bool WindowManager::Init()
    {
        if (m_mainWindow) return false;

        // 메인 윈도우 생성
        m_mainWindow = GLWindow::Create();
        if (!m_mainWindow)
        {
            BDT_LOG_ERROR("WindowManager: Failed to create main window.");
            return false;
        }

        // 메인 윈도우 컨텍스트 생성
        m_mainContext = GLMainContextUPtr(new GLMainContext(*m_mainWindow));
        return true;
    }

    void WindowManager::PollEvents()
    {
        m_mainWindow->PollGLFWEvents();
    }

    void WindowManager::Update()
    {
        if (!m_mainWindow) return;
        if (m_mainWindow->IsIconified()) return;
        m_mainWindow->SwapBuffers();
    }

    void WindowManager::Shutdown()
    {
        m_mainContext.reset();
        m_mainWindow.reset();
    }

    bool WindowManager::ShouldClose() const
    {
        if (!m_mainWindow) return true;
        return m_mainWindow->ShouldClose();
    }

    GLFWwindow* WindowManager::GetNativeWindow() const
    {
        if (!m_mainWindow) return nullptr;
        return m_mainWindow->GetHandle();
    }

    int32 WindowManager::GetWindowWidth() const
    {
        if (!m_mainWindow) return 0;
        return m_mainWindow->GetWidth();
    }

    int32 WindowManager::GetWindowHeight() const
    {
        if (!m_mainWindow) return 0;
        return m_mainWindow->GetHeight();
    }

    int32 WindowManager::GetFramebufferWidth() const
    {
        if (!m_mainWindow) return 0;
        return m_mainWindow->GetFramebufferWidth();
    }

    int32 WindowManager::GetFramebufferHeight() const
    {
        if (!m_mainWindow) return 0;
        return m_mainWindow->GetFramebufferHeight();
    }

    bool WindowManager::IsFramebufferResized() const
    {
        if (!m_mainWindow) return false;
        return m_mainWindow->IsFramebufferResized();
    }

    void WindowManager::ClearFramebufferResizeFlag()
    {
        if (m_mainWindow) m_mainWindow->ClearFramebufferResizeFlag();
    }
}