#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderTarget.h>
#include "Enums.h"

namespace bdt
{
    BDT_CLASS_PTR(MyGBuffer)
    BDT_CLASS_PTR(Framebuffer2D)
    BDT_CLASS_PTR(RenderTexture2D)

    class MyGBuffer : public IRenderTarget
    {
        BDT_DISABLE_COPY(MyGBuffer)
        BDT_DISABLE_MOVE(MyGBuffer)

    public:
        ~MyGBuffer() override;
        static MyGBufferUPtr Create(uint32 width, uint32 height);

        virtual bool Resize(uint32 width, uint32 height) override;
        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32 GetWidth() const override;
        virtual uint32 GetHeight() const override;

        void BindTextures() const;
        static void UnbindTextures();
        RenderTexture2D* GetTexture(EGBufferTextureSlot slot) const;
        bool IsValid() const;

    private:
        MyGBuffer();
        bool Init(uint32 width, uint32 height) override;

        // OpenGL GPU 제한 인자
        GLint m_maxTextureSize{ 0 };
        GLint m_maxColorattachments{ 0 };
        GLint m_maxDrawBuffers{ 0 };

        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
        Framebuffer2DUPtr m_framebuffer{ nullptr };
        Array<RenderTexture2DUPtr, (usize)EGBufferTextureSlot::Count> m_textures{};
    };
}
