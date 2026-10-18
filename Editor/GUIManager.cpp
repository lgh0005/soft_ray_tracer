#include "pch.h"
#include "GUIManager.h"

namespace bdt
{
    GUIManager::GUIManager() = default;
    GUIManager::~GUIManager() = default;

    bool GUIManager::Init()
    {
        if (m_context) return true;

        GLFWwindow* window = BDT_WINDOW.GetNativeWindow();
        if (!window || glfwGetCurrentContext() != window)
        {
            BDT_LOG_ERROR("GUIManager: Main window OpenGL context must be current before Init.");
            return false;
        }

        // 다른 코드가 소유한 ImGui 컨텍스트를 덮어쓰지 않는다.
        if (ImGui::GetCurrentContext())
        {
            BDT_LOG_ERROR("GUIManager: An ImGui context already exists.");
            return false;
        }

        if (!IMGUI_CHECKVERSION()) return false;
        m_context = ImGui::CreateContext();
        if (!m_context) return false;

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        // 기존 GLFW 콜백을 체이닝하여 창/입력 처리를 유지한다.
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
        {
            BDT_LOG_ERROR("GUIManager: Failed to initialize ImGui GLFW backend.");
            ImGui::DestroyContext(m_context);
            m_context = nullptr;
            return false;
        }

        if (!ImGui_ImplOpenGL3_Init("#version 460 core"))
        {
            BDT_LOG_ERROR("GUIManager: Failed to initialize ImGui OpenGL backend.");
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext(m_context);
            m_context = nullptr;
            return false;
        }

        return true;
    }

    void GUIManager::Render()
    {
        // TODO : 등록된 패널들을 렌더링한다.
    }

    void GUIManager::Shutdown()
    {
        if (!m_context) return;

        ImGuiContext* previousContext = ImGui::GetCurrentContext();
        ImGui::SetCurrentContext(m_context);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_context);

        if (previousContext != m_context)
            ImGui::SetCurrentContext(previousContext);
        m_context = nullptr;
    }
}
