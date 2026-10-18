#pragma once
#include "IRenderTarget.h"

namespace bdt
{
    BDT_CLASS_PTR(Framebuffer2D)
    BDT_CLASS_PTR(GLFramebuffer2D)

    class Framebuffer2D : public IRenderTarget
    {
        BDT_DISABLE_COPY(Framebuffer2D)
        BDT_DISABLE_MOVE(Framebuffer2D)

    public:
        ~Framebuffer2D() override;
        static Framebuffer2DUPtr Create(uint32 width, uint32 height);

        virtual bool Resize(uint32 width, uint32 height) override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32 GetWidth() const override;
        virtual uint32 GetHeight() const override;

        bool AttachColor(uint32 index, uint32 textureID);
        bool AttachDepth(uint32 textureID);
        bool AttachDepthStencil(uint32 textureID);
        
        void SetDrawBuffers(uint32 count);
        bool IsComplete() const;

    private:
        Framebuffer2D();
        bool Init(uint32 width, uint32 height);

    private:
        GLFramebuffer2DUPtr m_framebuffer{ nullptr };
    };
}