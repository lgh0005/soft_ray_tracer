#pragma once
#include "IRenderTarget.h"

namespace bdt
{
    BDT_CLASS_PTR(FramebufferCube)
    BDT_CLASS_PTR(GLFramebufferCube)

    class FramebufferCube : public IRenderTarget
    {
        BDT_DISABLE_COPY(FramebufferCube)
        BDT_DISABLE_MOVE(FramebufferCube)

    public:
        ~FramebufferCube() override;
        static FramebufferCubeUPtr Create(uint32 width, uint32 height);

        virtual bool Resize(uint32 width, uint32 height) override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32 GetWidth() const override;
        virtual uint32 GetHeight() const override;

        bool AttachColorFace(uint32 textureID, uint32 faceIndex, uint32 mipLevel = 0);
        bool AttachDepth(uint32 textureID);
        bool AttachDepthStencil(uint32 textureID);
        void SetDrawBuffers(uint32 count);
        bool IsComplete() const;

    private:
        FramebufferCube();
        bool Init(uint32 width, uint32 height);

    private:
        GLFramebufferCubeUPtr m_framebuffer{ nullptr };
    };
}