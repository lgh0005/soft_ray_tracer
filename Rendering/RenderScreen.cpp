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

    bool RenderScreen::Init(uint32 width, uint32 height)
    {
        return Resize(width, height);
    }

    bool RenderScreen::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        if (m_width == width || m_height == height) return true;
        m_width = width;
        m_height = height;
        return true;
    }

    void RenderScreen::Bind() const
    {
        // 기본 프레임버퍼 바인딩
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));
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

    void RenderScreen::Draw() const
    {
        GLVertexArray* vao = BDT_RENDER.GetScreenVAO();
        ScreenMeshPtr screenMesh = BDT_RENDER.GetScreenMesh();
        if (!vao || !screenMesh) return;

        const GLMesh* mesh = screenMesh->GetGLMesh();
        if (!mesh) return;

        const GLBuffer* vertexBuffer = mesh->GetVertexBuffer();
        const GLBuffer* indexBuffer = mesh->GetIndexBuffer();

        if (!vertexBuffer || !indexBuffer) return;

        vao->SetVertexBuffer(*vertexBuffer, 0, sizeof(GLScreenVertex));
        vao->SetIndexBuffer(*indexBuffer);

        vao->Bind();
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
