#include "pch.h"
#include "StaticModelRenderer.h"

namespace bdt
{
    StaticModelRenderer::StaticModelRenderer
    (
        Actor* owner,
        const ResourceHandle<Model>& model
    ) : Super(owner, model) { }

    StaticModelRenderer::~StaticModelRenderer() = default;
}
