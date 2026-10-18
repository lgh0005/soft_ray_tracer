#include "pch.h"
#include "GLContext.h"

namespace bdt
{
    void GLContext::MakeCurrent() const
    {
        glfwMakeContextCurrent(GetHandle());
    }

    void GLContext::ReleaseCurrent() const
    {
        if (IsCurrent()) glfwMakeContextCurrent(nullptr);
    }

    bool GLContext::IsCurrent() const
    {
        return glfwGetCurrentContext() == GetHandle();
    }
}