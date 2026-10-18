#include "pch.h"
#include "Framebuffer2D.h"
#include <Core/GLFramebuffer2D.h>

namespace bdt
{
	Framebuffer2D::Framebuffer2D() = default;
	Framebuffer2D::~Framebuffer2D() = default;

    Framebuffer2DUPtr Framebuffer2D::Create(uint32 width, uint32 height)
    {
        Framebuffer2DUPtr framebuffer(new Framebuffer2D());
        if (!framebuffer->Init(width, height)) return nullptr;
        return framebuffer;
    }

    bool Framebuffer2D::Init(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        GLFramebuffer2DUPtr framebuffer = GLFramebuffer2D::Create(width, height);
        if (!framebuffer || !framebuffer->IsValid()) return false;
        m_framebuffer = std::move(framebuffer);
        return true;
    }

    bool Framebuffer2D::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        if (width == GetWidth() && height == GetHeight()) return true;
        return Init(width, height);
    }

    void Framebuffer2D::Bind() const
    {
        if (!m_framebuffer) return;
        m_framebuffer->Bind();
    }

    void Framebuffer2D::Unbind() const
    {
        GLFramebuffer2D::Unbind();
    }

    uint32 Framebuffer2D::GetWidth() const
    {
        return m_framebuffer ? m_framebuffer->GetWidth() : 0;
    }

    uint32 Framebuffer2D::GetHeight() const
    {
        return m_framebuffer ? m_framebuffer->GetHeight() : 0;
    }

    bool Framebuffer2D::AttachColor(uint32 index, uint32 textureID)
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->AttachColor(index, textureID);
    }

    bool Framebuffer2D::AttachDepth(uint32 textureID)
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->AttachDepth(textureID);
    }

    bool Framebuffer2D::AttachDepthStencil(uint32 textureID)
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->AttachDepthStencil(textureID);
    }

    void Framebuffer2D::SetDrawBuffers(uint32 count)
    {
        if (!m_framebuffer) return;
        m_framebuffer->SetDrawBuffers(count);
    }

    bool Framebuffer2D::IsComplete() const
    {
        if (!m_framebuffer) return false;
        return m_framebuffer->IsComplete();
    }
}
