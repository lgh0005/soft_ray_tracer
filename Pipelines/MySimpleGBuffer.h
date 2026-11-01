#pragma once
#include <Rendering/IRenderTarget.h>
#include "Enums.h"

namespace bdt
{
    BDT_CLASS_PTR(MySimpleGBuffer)
    BDT_CLASS_PTR(Framebuffer2D)
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(GLSampler)

    class MySimpleGBuffer : public IRenderTarget
    {
        BDT_DISABLE_COPY(MySimpleGBuffer)
        BDT_DISABLE_MOVE(MySimpleGBuffer)

    public:
        virtual ~MySimpleGBuffer() override;
        static MySimpleGBufferUPtr Create(uint32 width, uint32 height);

        virtual bool Resize(uint32 width, uint32 height) override;
        virtual void Bind() const override;
        virtual void Unbind() const override;
        void Clear() const;

        virtual uint32 GetWidth() const override;
        virtual uint32 GetHeight() const override;

        void BindTextures() const;
        static void UnbindTextures();
        RenderTexture2D* GetTexture(EGBufferTextureSlot slot) const;
        GLSampler* GetSampler() const;
        bool IsValid() const;

    private:
        MySimpleGBuffer();
        bool Init(uint32 width, uint32 height) override;

        // OpenGL GPU 제한 인자
        GLint m_maxTextureSize{ 0 };
        GLint m_maxColorattachments{ 0 };
        GLint m_maxDrawBuffers{ 0 };

        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
        Framebuffer2DUPtr m_framebuffer{ nullptr };
        GLSamplerUPtr m_sampler{ nullptr };
        Array<RenderTexture2DUPtr, (usize)EGBufferTextureSlot::Count> m_textures{};
    };
}
