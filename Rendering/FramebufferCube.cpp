#include "pch.h"
#include "FramebufferCube.h"
#include <Core/GLFramebufferCube.h>

namespace bdt
{
    FramebufferCube::FramebufferCube() = default;
    FramebufferCube::~FramebufferCube() = default;

    FramebufferCubeUPtr FramebufferCube::Create(uint32 width, uint32 height)
    {
        FramebufferCubeUPtr framebuffer(new FramebufferCube());
        if (!framebuffer->Init(width, height)) return nullptr;
        return framebuffer;
    }

    bool FramebufferCube::Init(uint32 width, uint32 height)
    {
        if (width == 0 || width != height) return false;
        GLFramebufferCubeUPtr framebuffer = GLFramebufferCube::Create(width);
        if (!framebuffer || !framebuffer->IsValid()) return false;
        m_framebuffer = std::move(framebuffer);
        return true;
    }

    bool FramebufferCube::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || width != height) return false;
        if (width == GetWidth() && height == GetHeight()) return true;
        return Init(width, height);
    }

    void FramebufferCube::Bind() const
    {
        if (!m_framebuffer) return;
        m_framebuffer->Bind();
    }

    void FramebufferCube::Unbind() const
    {
        GLFramebufferCube::Unbind();
    }

    uint32 FramebufferCube::GetWidth() const
    {
        return m_framebuffer ? m_framebuffer->GetWidth() : 0;
    }

    uint32 FramebufferCube::GetHeight() const
    {
        return m_framebuffer ? m_framebuffer->GetHeight() : 0;
    }

    bool FramebufferCube::AttachColorFace(uint32 textureID, uint32 faceIndex, uint32 mipLevel)
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->AttachColorFace(textureID, faceIndex, mipLevel);
    }

    bool FramebufferCube::AttachDepth(uint32 textureID)
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->AttachDepth(textureID);
    }

    bool FramebufferCube::AttachDepthStencil(uint32 textureID)
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->AttachDepthStencil(textureID);
    }

    void FramebufferCube::SetDrawBuffers(uint32 count)
    {
        if (!m_framebuffer) return;
        m_framebuffer->SetDrawBuffers(count);
    }

    bool FramebufferCube::IsComplete() const
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->IsComplete();
    }
}
