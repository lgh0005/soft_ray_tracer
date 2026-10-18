#include "pch.h"
#include "GLFramebuffer2D.h"

namespace bdt
{
	GLFramebuffer2D::GLFramebuffer2D() = default;
	GLFramebuffer2D::~GLFramebuffer2D() { Clear(); }

    GLFramebuffer2DUPtr GLFramebuffer2D::Create(uint32 width, uint32 height)
    {
        GLFramebuffer2DUPtr framebuffer(new GLFramebuffer2D());
        if (!framebuffer->Init(width, height)) return nullptr;
        return framebuffer;
    }

    bool GLFramebuffer2D::Init(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;

        // Framebuffer 객체만 생성
        glCreateFramebuffers(1, &m_framebufferID);
        if (m_framebufferID == 0) return false;

        m_width = width;
        m_height = height;

        return true;
    }

    void GLFramebuffer2D::Clear()
    {
        if (m_framebufferID != 0)
        {
            glDeleteFramebuffers(1, &m_framebufferID);
            m_framebufferID = 0;
        }

        m_width = 0;
        m_height = 0;
    }

    void GLFramebuffer2D::Bind() const
    {
        if (!IsValid()) return;
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
    }

    void GLFramebuffer2D::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool GLFramebuffer2D::AttachColor(uint32 index, uint32 textureID)
    {
        if (!IsValid() || textureID == 0) return false;

        GLint maxAttachments = 0;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
        if (index >= static_cast<uint32>(maxAttachments))
            return false;

        glNamedFramebufferTexture(m_framebufferID, GL_COLOR_ATTACHMENT0 + index, textureID, 0);
        return true;
    }

    bool GLFramebuffer2D::AttachDepth(uint32 textureID)
    {
        if (!IsValid() || textureID == 0) return false;
        glNamedFramebufferTexture(m_framebufferID, GL_DEPTH_ATTACHMENT, textureID, 0);
        return true;
    }

    bool GLFramebuffer2D::AttachStencil(uint32 textureID)
    {
        if (!IsValid() || textureID == 0) return false;
        glNamedFramebufferTexture(m_framebufferID, GL_STENCIL_ATTACHMENT, textureID,0);
        return true;
    }

    bool GLFramebuffer2D::AttachDepthStencil(uint32 textureID)
    {
        if (!IsValid() || textureID == 0) return false;
        glNamedFramebufferTexture(m_framebufferID, GL_DEPTH_STENCIL_ATTACHMENT, textureID, 0);
        return true;
    }

    void GLFramebuffer2D::SetDrawBuffers(uint32 count)
    {
        if (!IsValid()) return;
        if (count == 0)
        {
            glNamedFramebufferDrawBuffer(m_framebufferID, GL_NONE);
            glNamedFramebufferReadBuffer(m_framebufferID, GL_NONE);
            return;
        }

        GLint maxDrawBuffers = 0;
        GLint maxColorAttachments = 0;
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
        if (count > static_cast<uint32>(maxDrawBuffers) ||
            count > static_cast<uint32>(maxColorAttachments))
            return;

        List<GLenum> buffers;
        buffers.reserve(count);
        for (uint32 i = 0; i < count; ++i)
            buffers.push_back(GL_COLOR_ATTACHMENT0 + i);

        glNamedFramebufferDrawBuffers(m_framebufferID, static_cast<GLsizei>(buffers.size()), buffers.data());
        glNamedFramebufferReadBuffer(m_framebufferID, GL_COLOR_ATTACHMENT0);
    }

    bool GLFramebuffer2D::IsComplete() const
    {
        if (!IsValid()) return false;
        return glCheckNamedFramebufferStatus(m_framebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    uint32 GLFramebuffer2D::GetFramebufferID() const
    {
        return m_framebufferID;
    }

    uint32 GLFramebuffer2D::GetWidth() const
    {
        return m_width;
    }

    uint32 GLFramebuffer2D::GetHeight() const
    {
        return m_height;
    }

    bool GLFramebuffer2D::IsValid() const
    {
        return m_framebufferID != 0;
    }
}