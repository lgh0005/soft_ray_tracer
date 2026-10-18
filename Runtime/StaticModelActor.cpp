#include "pch.h"
#include "StaticModelActor.h"
#include "StaticModelRenderer.h"

namespace bdt
{
    StaticModelActor::StaticModelActor
    (
        const String& tag,
        const ResourceHandle<Model>& model
    ) : Super(tag)
    {
        m_rootComponent = std::make_unique<StaticModelRenderer>(this, model);
    }

    StaticModelActor::~StaticModelActor() = default;

    StaticModelRenderer* StaticModelActor::GetStaticModelRenderer() const
    {
        return static_cast<StaticModelRenderer*>(m_rootComponent.get());
    }
}
