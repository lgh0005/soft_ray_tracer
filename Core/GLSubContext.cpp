#include "pch.h"
#include "GLSubContext.h"
#include "GLMainContext.h"

namespace bdt
{
    thread_local GLSubContext* GLSubContext::s_current{ nullptr };

    GLSubContext::GLSubContext() = default;

    GLSubContext::~GLSubContext()
    {
        Destroy();
    }

    GLSubContextUPtr GLSubContext::Create(const GLMainContext& mainContext)
    {
        GLSubContextUPtr context(new GLSubContext());
        if (!context->Init(mainContext)) return nullptr;
        return context;
    }

    bool GLSubContext::Init(const GLMainContext& mainContext)
    {
        GLFWwindow* mainWindow = mainContext.GetHandle();
        if (!mainWindow) return false;

        // OpenGL 컨텍스트 설정
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, BDT_OPENGL_MAJOR_VERSION);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, BDT_OPENGL_MINOR_VERSION);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // 별도 윈도우를 표시하지 않음
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // 메인 컨텍스트와 GPU 객체 공유
        m_handle = glfwCreateWindow(1, 1, "GLSubContext", nullptr, mainWindow);

        // 다음 윈도우 생성에 영향이 없도록 복원
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

        return m_handle != nullptr;
    }

    bool GLSubContext::AttachToCurrentThread()
    {
        if (!m_handle) return false;

        GLFWwindow* current = glfwGetCurrentContext();

        // 이미 다른 컨텍스트가 바인딩되어 있다면 거부
        if (current && current != m_handle) return false;

        // 이미 다른 GLSubContext가 등록되어 있다면 거부
        if (s_current && s_current != this) return false;

        glfwMakeContextCurrent(m_handle);
        if (glfwGetCurrentContext() != m_handle)
            return false;

        s_current = this;
        return true;
    }

    void GLSubContext::DetachFromCurrentThread()
    {
        if (s_current != this) return;

        if (glfwGetCurrentContext() == m_handle)
            glfwMakeContextCurrent(nullptr);

        s_current = nullptr;
    }

    void GLSubContext::Destroy()
    {
        // 모든 워커가 DetachFromCurrentThread() 후 종료했으며,
        // 이 함수는 GLFW 메인 스레드에서 실행한다.
        if (!m_handle) return;
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }

    GLFWwindow* GLSubContext::GetHandle() const
    {
        return m_handle;
    }
}