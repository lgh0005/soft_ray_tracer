#pragma once
#include "Defines.h"
#include "Types.h"

struct GLFWwindow;

namespace bdt
{
    BDT_CLASS_PTR(GLWindow)
    BDT_CLASS_PTR(GLMainContext)

    class WindowManager
    {
        BDT_DECLARE_SINGLE(WindowManager)

    public:
        bool Init();
        void PollEvents();
        void Update();
        void Shutdown();

    public:
        bool ShouldClose() const;
        GLWindow* GetMainWindow() const { return m_mainWindow.get(); }
        GLMainContext* GetMainContext() const { return m_mainContext.get(); }
        GLFWwindow* GetNativeWindow() const;

        int32 GetWindowWidth() const;
        int32 GetWindowHeight() const;
        int32 GetFramebufferWidth() const;
        int32 GetFramebufferHeight() const;

        bool IsFramebufferResized() const;
        void ClearFramebufferResizeFlag();

    private:
        GLWindowUPtr m_mainWindow{ nullptr };
        GLMainContextUPtr m_mainContext{ nullptr };
    };
}