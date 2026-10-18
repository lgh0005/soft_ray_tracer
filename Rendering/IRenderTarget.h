#pragma once

namespace bdt
{
    class IRenderTarget
    {
    public:
        virtual ~IRenderTarget() = default;

        virtual bool Init(uint32 width, uint32 height) = 0;
        virtual bool Resize(uint32 width, uint32 height) = 0;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual uint32 GetWidth() const = 0;
        virtual uint32 GetHeight() const = 0;
    };
}

