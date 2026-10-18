#pragma once

namespace bdt
{
    class RenderViewport
    {
    public:
        RenderViewport();
        ~RenderViewport();

    public:
        void Resize(uint32 width, uint32 height);
        uint32 GetWidth() const;
        uint32 GetHeight() const;
        float GetAspectRatio() const;

    private:
        uint32 m_width  { 1 };
        uint32 m_height { 1 };
    };
}