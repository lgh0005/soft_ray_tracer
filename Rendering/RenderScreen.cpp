#include "pch.h"
#include "RenderScreen.h"
#include "Rendering.h"

#include <Core/GLVertexArray.h>
#include <Core/GLBuffer.h>
#include <Core/GLMesh.h>
#include <Core/GLVertices.h>
#include <Resources/ScreenMesh.h>

namespace bdt
{
    RenderScreen::RenderScreen() = default;
    RenderScreen::~RenderScreen() = default;

    RenderScreenUPtr RenderScreen::Create(uint32 width, uint32 height)
    {
        RenderScreenUPtr screen = RenderScreenUPtr(new RenderScreen());
        if (!screen->Init(width, height)) return nullptr;
        return screen;
    }

    bool RenderScreen::Init(uint32 width, uint32 height)
    {
        m_screenMesh = ScreenMesh::Create();
        if (!m_screenMesh) return false;

        m_screenVAO = GLVertexArray::Create();
        if (!m_screenVAO) return false;

        m_screenVAO->SetAttrib(0, 0, 3, GL_FLOAT, false, offsetof(GLScreenVertex, position));
        m_screenVAO->SetAttrib(1, 0, 2, GL_FLOAT, false, offsetof(GLScreenVertex, texCoord));

        return Resize(width, height);
    }

    bool RenderScreen::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        if (m_width == width && m_height == height) return true;

        // 1. HDR Color Texture 생성
        auto colorTexture = RenderTexture2D::Create(width, height, GL_RGBA16F);
        if (!colorTexture) return false;

        // 2. Framebuffer 생성
        auto framebuffer = Framebuffer2D::Create(width, height);
        if (!framebuffer) return false;

        // 3. Color Attachment 연결
        if (!framebuffer->AttachColor(0, colorTexture->GetTextureID()))
            return false;

        framebuffer->SetDrawBuffers(1);

        // 4. 기존 리소스 교체
        m_colorTexture = std::move(colorTexture);
        m_framebuffer = std::move(framebuffer);

        m_width = width;
        m_height = height;

        return true;
    }

    void RenderScreen::Bind() const
    {
        m_framebuffer->Bind();
        glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
    }

    void RenderScreen::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    uint32 RenderScreen::GetWidth() const
    {
        return m_width;
    }

    uint32 RenderScreen::GetHeight() const
    {
        return m_height;
    }

    RenderTexture2D* RenderScreen::GetColorTexture() const
    {
        return m_colorTexture.get();
    }

    void RenderScreen::ScreenBlit() const
    {
        const GLMesh* mesh = m_screenMesh->GetGLMesh();
        if (!mesh) return;

        const GLBuffer* vertexBuffer = mesh->GetVertexBuffer();
        const GLBuffer* indexBuffer = mesh->GetIndexBuffer();
        if (!vertexBuffer || !indexBuffer) return;

        m_screenVAO->SetVertexBuffer(*vertexBuffer, 0, sizeof(GLScreenVertex));
        m_screenVAO->SetIndexBuffer(*indexBuffer);
        m_screenVAO->Bind();
        
        glDrawElements
        (
            mesh->GetPrimitive(),
            static_cast<GLsizei>(mesh->GetIndexCount()),
            GL_UNSIGNED_INT,
            nullptr
        );

        GLVertexArray::Unbind();
    }
}
