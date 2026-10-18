#include "pch.h"
#include "IRenderBus.h"

namespace bdt
{
    IRenderBus::IRenderBus() = default;
    IRenderBus::~IRenderBus() = default;

    void IRenderBus::SetRenderScene(const RenderScene* scene)
    {
        m_renderScene = scene;
    }

    const RenderScene* IRenderBus::GetRenderScene() const
    {
        return m_renderScene;
    }
}