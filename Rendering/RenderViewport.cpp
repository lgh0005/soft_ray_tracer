#include "pch.h"
#include "RenderViewport.h"

namespace bdt
{
    RenderViewport::RenderViewport() = default;
    RenderViewport::~RenderViewport() = default;

    void RenderViewport::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return;
        m_width = width;
        m_height = height;
    }

    uint32 RenderViewport::GetWidth() const
    {
        return m_width;
    }

    uint32 RenderViewport::GetHeight() const
    {
        return m_height;
    }

    float RenderViewport::GetAspectRatio() const
    {
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }
}
