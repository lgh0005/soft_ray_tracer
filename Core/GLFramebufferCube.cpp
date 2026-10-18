#include "pch.h"
#include "GLFramebufferCube.h"

namespace bdt
{
	GLFramebufferCube::GLFramebufferCube() = default;
	GLFramebufferCube::~GLFramebufferCube() { Clear(); }

    GLFramebufferCubeUPtr GLFramebufferCube::Create(uint32 size)
    {
        GLFramebufferCubeUPtr framebuffer(new GLFramebufferCube());
        if (!framebuffer->Init(size)) return nullptr;
        return framebuffer;
    }

    bool GLFramebufferCube::Init(uint32 size)
    {
        if (size == 0) return false;

        glCreateFramebuffers(1, &m_framebufferID);
        if (m_framebufferID == 0) return false;

        m_size = size;
        return true;
    }

    void GLFramebufferCube::Clear()
    {
        if (m_framebufferID != 0)
        {
            glDeleteFramebuffers(1, &m_framebufferID);
            m_framebufferID = 0;
        }

        m_size = 0;
    }

    void GLFramebufferCube::Bind() const
    {
        if (!IsValid()) return;
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
    }

    void GLFramebufferCube::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool GLFramebufferCube::AttachColorFace(uint32 textureID, uint32 faceIndex, uint32 mipLevel)
    {
        if (!IsValid() || textureID == 0 || faceIndex >= 6)
            return false;

        glNamedFramebufferTextureLayer
        (
            m_framebufferID,
            GL_COLOR_ATTACHMENT0,
            textureID,
            static_cast<GLint>(mipLevel),
            static_cast<GLint>(faceIndex)
        );

        return true;
    }

    bool GLFramebufferCube::AttachDepth(uint32 textureID)
    {
        if (!IsValid() || textureID == 0) return false;
        glNamedFramebufferTexture(m_framebufferID, GL_DEPTH_ATTACHMENT, textureID, 0);
        return true;
    }

    bool GLFramebufferCube::AttachDepthStencil(uint32 textureID)
    {
        if (!IsValid() || textureID == 0) return false;
        glNamedFramebufferTexture(m_framebufferID, GL_DEPTH_STENCIL_ATTACHMENT, textureID, 0);
        return true;
    }

    void GLFramebufferCube::SetDrawBuffers(uint32 count)
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

    uint32 GLFramebufferCube::GetFramebufferID() const
    {
        return m_framebufferID;
    }

    uint32 GLFramebufferCube::GetWidth() const
    {
        return m_size;
    }

    uint32 GLFramebufferCube::GetHeight() const
    {
        return m_size;
    }

    bool GLFramebufferCube::IsValid() const
    {
        return m_framebufferID != 0;
    }
}