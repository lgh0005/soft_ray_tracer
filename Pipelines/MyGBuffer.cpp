#include "pch.h"
#include "MyGBuffer.h"
#include <Rendering/Framebuffer2D.h>
#include <Rendering/RenderTexture2D.h>

namespace bdt
{
	MyGBuffer::MyGBuffer() = default;
	MyGBuffer::~MyGBuffer() = default;

    bool MyGBuffer::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        if (width == m_width && height == m_height) return true;
        return Init(width, height);
    }

    void MyGBuffer::Bind() const
    {
        if (!m_framebuffer) return;
        m_framebuffer->Bind();
    }

    void MyGBuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    uint32 MyGBuffer::GetWidth() const
    {
        return m_width;
    }

    uint32 MyGBuffer::GetHeight() const
    {
        return m_height;
    }

    void MyGBuffer::BindTextures() const
    {
        for (uint32 slot = 0; slot < static_cast<uint32>(EGBufferTextureSlot::Count); ++slot)
            m_textures[slot]->Bind(slot);
    }

    void MyGBuffer::UnbindTextures()
    {
        for (uint32 slot = 0; slot < static_cast<uint32>(EGBufferTextureSlot::Count); ++slot)
            RenderTexture2D::Unbind(slot);
    }

    RenderTexture2D* MyGBuffer::GetTexture(EGBufferTextureSlot slot) const
    {
        const usize index = static_cast<usize>(slot);
        if (index >= m_textures.size()) return nullptr;
        return m_textures[index].get();
    }

    bool MyGBuffer::IsValid() const
    {
        if (!m_framebuffer) return false;

        for (const auto& texture : m_textures)
        {
            if (!texture || !texture->IsValid())
                return false;
        }

        return true;
    }

    MyGBufferUPtr MyGBuffer::Create(uint32 width, uint32 height)
    {
        MyGBufferUPtr gBuffer(new MyGBuffer());
        if (!gBuffer->Init(width, height)) return nullptr;
        return gBuffer;
    }

    bool MyGBuffer::Init(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;

        // RenderManager에 캐싱된 OpenGL 제한값 사용
        const GLint maxTextureSize = BDT_RENDER.GetMaxTextureSize();
        const GLint maxColorAttachments = BDT_RENDER.GetMaxColorAttachments();
        const GLint maxDrawBuffers = BDT_RENDER.GetMaxDrawBuffers();
        const uint32 colorCount = static_cast<uint32>(EGBufferTextureSlot::Depth);
        if (maxTextureSize <= 0                                  ||
            width > static_cast<uint32>(maxTextureSize)          ||
            height > static_cast<uint32>(maxTextureSize)         ||
            maxColorAttachments < static_cast<GLint>(colorCount) ||
            maxDrawBuffers < static_cast<GLint>(colorCount))
        {
            return false;
        }

        // 1. GBuffer 텍스처 생성
        Array<RenderTexture2DUPtr, (usize)EGBufferTextureSlot::Count> textures{};
        textures[(usize)EGBufferTextureSlot::Position] = RenderTexture2D::Create(width, height, GL_RGBA16F);
        textures[(usize)EGBufferTextureSlot::Normal] = RenderTexture2D::Create(width, height, GL_RGBA16F);
        textures[(usize)EGBufferTextureSlot::Albedo] = RenderTexture2D::Create(width, height, GL_RGBA8);
        textures[(usize)EGBufferTextureSlot::Material] = RenderTexture2D::Create(width, height, GL_RGBA8);
        textures[(usize)EGBufferTextureSlot::Emission] = RenderTexture2D::Create(width, height, GL_RGBA16F);
        textures[(usize)EGBufferTextureSlot::Depth] = RenderTexture2D::Create(width, height, GL_DEPTH_COMPONENT32F);
        for (const auto& texture : textures)
        {
            if (!texture || !texture->IsValid())
                return false;

            const uint32 textureID = texture->GetTextureID();
            glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        // 2. Framebuffer 생성
        auto framebuffer = Framebuffer2D::Create(width, height);
        if (!framebuffer) return false;

        // 3. Color Attachments 연결
        for (uint32 i = 0; i < colorCount; ++i)
        {
            if (!framebuffer->AttachColor(i, textures[i]->GetTextureID()))
                return false;
        }

        // 4. Depth 연결
        if (!framebuffer->AttachDepth(textures[(usize)EGBufferTextureSlot::Depth]->GetTextureID()))
            return false;

        // 기본 설정은 attachment 0만 출력하므로 모든 Color Attachment 출력을 명시한다.
        framebuffer->SetDrawBuffers(colorCount);

        // 5. Framebuffer 완전성 검사
        if (!framebuffer) return false;

        m_framebuffer = std::move(framebuffer);
        m_textures = std::move(textures);
        m_width = width;
        m_height = height;

        return true;
    }
}
