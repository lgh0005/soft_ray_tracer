#pragma once
#include "ModelRenderer.h"

namespace bdt
{
    class StaticModelRenderer : public ModelRenderer
    {
        using Super = ModelRenderer;

    public:
        explicit StaticModelRenderer
        (
            Actor* owner,
            const ResourceHandle<Model>& model
        );
        virtual ~StaticModelRenderer() override;

    };
}