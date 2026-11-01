#pragma once
#include "IRenderTarget.h"

namespace bdt
{
    BDT_CLASS_PTR(ScreenMesh)
    BDT_CLASS_PTR(GLVertexArray)
    BDT_CLASS_PTR(Framebuffer2D)
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(RenderScreen)

    class RenderScreen : public IRenderTarget
    {
        BDT_DISABLE_COPY(RenderScreen)
        BDT_DISABLE_MOVE(RenderScreen)

    public:
        virtual ~RenderScreen() override;
        static RenderScreenUPtr Create(uint32 width, uint32 height);
        virtual bool Resize(uint32 width, uint32 height) override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32 GetWidth() const override;
        virtual uint32 GetHeight() const override;

        RenderTexture2D* GetColorTexture() const;
        void ScreenBlit() const;

    private:
        RenderScreen();
        virtual bool Init(uint32 width, uint32 height) override;

        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
   
        Framebuffer2DUPtr m_framebuffer{ nullptr };
        RenderTexture2DUPtr m_colorTexture{ nullptr };

        GLVertexArrayUPtr m_screenVAO{ nullptr };
        ScreenMeshPtr m_screenMesh{ nullptr };
    };
}
