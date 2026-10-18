
#pragma once
#include "IRenderTarget.h"

namespace bdt
{
    BDT_CLASS_PTR(ScreenMesh)
    BDT_CLASS_PTR(GLVertexArray)

    class RenderScreen final : public IRenderTarget
    {
        BDT_DISABLE_COPY(RenderScreen)
        BDT_DISABLE_MOVE(RenderScreen)

    public:
        RenderScreen();
        ~RenderScreen() override;

        virtual bool Init(uint32 width, uint32 height) override;
        virtual bool Resize(uint32 width, uint32 height) override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32 GetWidth() const override;
        virtual uint32 GetHeight() const override;

        void Draw() const;

    private:
        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
    };
}
