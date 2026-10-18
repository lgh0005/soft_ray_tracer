#include "pch.h"
#include "GLMainContext.h"
#include "GLWindow.h"

namespace bdt
{
    GLMainContext::GLMainContext(GLWindow& window) : m_window(&window) { }

    GLFWwindow* GLMainContext::GetHandle() const
    {
        return m_window->GetHandle();
    }
}