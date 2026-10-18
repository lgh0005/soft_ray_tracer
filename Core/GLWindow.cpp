#include "pch.h"
#include "GLWindow.h"
#include "Logger.h"

namespace bdt
{
	GLWindow::GLWindow() = default;
    GLWindow::~GLWindow() { Destroy(); }

    GLWindowUPtr GLWindow::Create()
    {
        GLWindowUPtr window(new GLWindow());
        if (!window->Init()) return nullptr;
        return window;
    }

    bool GLWindow::Init()
    {
        // glfw 초기화
        if (glfwInit() != GLFW_TRUE)
            return false;

        // OpenGL 컨텍스트 설정
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, BDT_OPENGL_MAJOR_VERSION);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, BDT_OPENGL_MINOR_VERSION);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // 윈도우 생성
        GLFWwindow* window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!window) return false;
        
        m_handle = window;

        // OpenGL 컨텍스트 활성화
        glfwMakeContextCurrent(window);

        // 윈도우 초기 상태
        glfwGetWindowSize(window, &m_width, &m_height);
        glfwGetFramebufferSize(window, &m_framebufferWidth, &m_framebufferHeight);
        m_isIconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE;

        // GLFW 윈도우와 C++ 인스턴스 연결
        glfwSetWindowUserPointer(window, this);

        // GLAD 초기화
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        {
            BDT_LOG_ERROR("GLWindow: Failed to initialize GLAD.");
            return false;
        }

        // 윈도우 콜백 등록
        glfwSetWindowPosCallback(m_handle, HandleWindowPosition);
        glfwSetWindowSizeCallback(m_handle, HandleWindowSize);
        glfwSetWindowCloseCallback(m_handle, HandleWindowClose);
        glfwSetWindowRefreshCallback(m_handle, HandleWindowRefresh);
        glfwSetWindowFocusCallback(m_handle, HandleWindowFocus);
        glfwSetWindowIconifyCallback(m_handle, HandleWindowIconify);
        glfwSetWindowMaximizeCallback(m_handle, HandleWindowMaximize);
        glfwSetFramebufferSizeCallback(m_handle, HandleFramebufferSize);
        glfwSetWindowContentScaleCallback(m_handle, HandleContentScale);

        // 키보드 콜백 등록
        glfwSetKeyCallback(m_handle, HandleKey);
        glfwSetCharCallback(m_handle, HandleChar);

        // 마우스 콜백 등록
        glfwSetMouseButtonCallback(m_handle, HandleMouseButton);
        glfwSetCursorPosCallback(m_handle, HandleCursorPosition);
        glfwSetCursorEnterCallback(m_handle, HandleCursorEnter);
        glfwSetScrollCallback(m_handle, HandleScroll);

        // 파일 콜백 등록
        glfwSetDropCallback(m_handle, HandleFileDrop);

        return true;
    }

    void GLWindow::Destroy()
    {
        if (!m_handle) return;
        glfwSetWindowUserPointer(m_handle, nullptr);
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }

    void GLWindow::PollGLFWEvents()
    {
        glfwPollEvents();
    }

    void GLWindow::SwapBuffers()
    {
        if (!m_handle) return;
        glfwSwapBuffers(m_handle);
    }

    bool GLWindow::ShouldClose() const
    {
        if (!m_handle) return true;
        return glfwWindowShouldClose(m_handle) == GLFW_TRUE;
    }

    void GLWindow::RequestClose(bool shouldClose)
    {
        if (!m_handle) return;
        glfwSetWindowShouldClose(m_handle, shouldClose ? GLFW_TRUE : GLFW_FALSE);
    }

    /*==========================================//
    //              GLFW Callbacks              //
    //==========================================*/
    GLWindow* GLWindow::GetGLFWWindow(GLFWwindow* window)
    {
        return static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    }

    // 윈도우 위치 변경
    void GLWindow::HandleWindowPosition(GLFWwindow* window, int32 x, int32 y)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 윈도우 위치 변경 이벤트 처리
    }

    // 윈도우 크기 변경
    void GLWindow::HandleWindowSize(GLFWwindow* window, int32 width, int32 height)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        self->m_width = width;
        self->m_height = height;
    }

    // 윈도우 닫기 요청
    void GLWindow::HandleWindowClose(GLFWwindow* window)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        self->RequestClose(true);
    }

    // 윈도우 새로고침
    void GLWindow::HandleWindowRefresh(GLFWwindow* window)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 화면 갱신 이벤트 처리
    }

    // 윈도우 포커스 변경
    void GLWindow::HandleWindowFocus(GLFWwindow* window, int32 focused)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 포커스 변경 이벤트 처리
    }

    // 윈도우 최소화 및 복원
    void GLWindow::HandleWindowIconify(GLFWwindow* window, int32 iconified)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        self->m_isIconified = (iconified == GLFW_TRUE);
    }

    // 윈도우 최대화 및 복원
    void GLWindow::HandleWindowMaximize(GLFWwindow* window, int32 maximized)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 최대화 이벤트 처리
    }

    // 프레임버퍼 크기 변경
    void GLWindow::HandleFramebufferSize(GLFWwindow* window, int32 width, int32 height)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        if (self->m_framebufferWidth == width && self->m_framebufferHeight == height) 
            return;

        self->m_framebufferWidth = width;
        self->m_framebufferHeight = height;
        self->m_framebufferResized = true;
    }

    // DPI 콘텐츠 스케일 변경
    void GLWindow::HandleContentScale(GLFWwindow* window, float xscale, float yscale)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: DPI 변경 이벤트 처리
    }

    // 키보드 입력
    void GLWindow::HandleKey(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: InputManager에 키보드 입력 전달
    }

    // 유니코드 문자 입력
    void GLWindow::HandleChar(GLFWwindow* window, unsigned int codepoint)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 문자 입력 이벤트 처리
    }

    // 마우스 버튼 입력
    void GLWindow::HandleMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mods)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: InputManager에 마우스 버튼 입력 전달
    }

    // 마우스 커서 이동
    void GLWindow::HandleCursorPosition(GLFWwindow* window, double x, double y)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: InputManager에 마우스 위치 전달
    }

    // 마우스 커서 진입 및 이탈
    void GLWindow::HandleCursorEnter(GLFWwindow* window, int32 entered)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 마우스 커서 진입 및 이탈 이벤트 처리
    }

    // 마우스 스크롤
    void GLWindow::HandleScroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: InputManager에 스크롤 입력 전달
    }

    // 파일 드래그 앤 드롭
    void GLWindow::HandleFileDrop(GLFWwindow* window, int32 count, const char** paths)
    {
        GLWindow* self = GetGLFWWindow(window);
        if (!self) return;

        // TODO: 파일 드롭 이벤트 처리
    }
}