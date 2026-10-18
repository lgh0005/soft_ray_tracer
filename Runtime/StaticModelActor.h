#pragma once
#include "ModelActor.h"

namespace bdt
{
    BDT_CLASS_PTR(Model)
    BDT_CLASS_PTR(StaticModelRenderer)

    class StaticModelActor : public ModelActor
    {
        using Super = ModelActor;

    public:
        StaticModelActor
        (
            const String& tag, 
            const ResourceHandle<Model>& model
        );
        virtual ~StaticModelActor() override;

    public:
        StaticModelRenderer* GetStaticModelRenderer() const;
    };
}
