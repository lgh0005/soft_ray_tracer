#include "pch.h"
#include "MySimpleGBuffer.h"

namespace bdt
{
	MySimpleGBuffer::MySimpleGBuffer() = default;
	MySimpleGBuffer::~MySimpleGBuffer() = default;

    bool MySimpleGBuffer::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return false;
        if (width == m_width && height == m_height) return true;
        return Init(width, height);
    }

    void MySimpleGBuffer::Bind() const
    {
        if (!m_framebuffer) return;
        m_framebuffer->Bind();
    }

    void MySimpleGBuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MySimpleGBuffer::Clear() const
    {
        // 컬러 버퍼 초기화
        const GLfloat clearColor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
        for (GLint i = 0; i < static_cast<GLint>(EGBufferTextureSlot::Depth); ++i)
            glClearBufferfv(GL_COLOR, i, clearColor);

        // 깊이 버퍼 초기화
        const GLfloat clearDepth = 1.0f;
        glClearBufferfv(GL_DEPTH, 0, &clearDepth);
    }

    uint32 MySimpleGBuffer::GetWidth() const
    {
        return m_width;
    }

    uint32 MySimpleGBuffer::GetHeight() const
    {
        return m_height;
    }

    void MySimpleGBuffer::BindTextures() const
    {
        for (uint32 slot = 0; slot < static_cast<uint32>(EGBufferTextureSlot::Count); ++slot)
        {
            m_textures[slot]->Bind(slot);
            m_sampler->Bind(slot);
        }
    }

    void MySimpleGBuffer::UnbindTextures()
    {
        for (uint32 slot = 0; slot < static_cast<uint32>(EGBufferTextureSlot::Count); ++slot)
        {
            RenderTexture2D::Unbind(slot);
            GLSampler::Unbind(slot);
        }
    }

    RenderTexture2D* MySimpleGBuffer::GetTexture(EGBufferTextureSlot slot) const
    {
        const usize index = static_cast<usize>(slot);
        if (index >= m_textures.size()) return nullptr;
        return m_textures[index].get();
    }

    GLSampler* MySimpleGBuffer::GetSampler() const
    {
        return m_sampler.get();
    }

    bool MySimpleGBuffer::IsValid() const
    {
        if (!m_framebuffer || !m_sampler || !m_sampler->IsValid()) return false;

        for (const auto& texture : m_textures)
        {
            if (!texture || !texture->IsValid())
                return false;
        }

        return true;
    }

    MySimpleGBufferUPtr MySimpleGBuffer::Create(uint32 width, uint32 height)
    {
        MySimpleGBufferUPtr gBuffer(new MySimpleGBuffer());
        if (!gBuffer->Init(width, height)) return nullptr;
        return gBuffer;
    }

    bool MySimpleGBuffer::Init(uint32 width, uint32 height)
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
        }

        auto sampler = GLSampler::Create();
        if (!sampler) return false;
        sampler->SetFilter(GL_NEAREST, GL_NEAREST);
        sampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

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
        m_sampler = std::move(sampler);
        m_textures = std::move(textures);
        m_width = width;
        m_height = height;

        return true;
    }
}
