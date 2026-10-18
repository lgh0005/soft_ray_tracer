#pragma once
#include "ModelActor.h"
#include <Resources/ResourceHandle.h>

namespace bdt
{
    BDT_CLASS_PTR(Model)
    BDT_CLASS_PTR(SkinnedModelRenderer)
    BDT_CLASS_PTR(AnimComponent)

    class SkinnedModelActor : public ModelActor
    {
        using Super = ModelActor;

    public:
        SkinnedModelActor
        (
            const String& tag,
            const ResourceHandle<Model>& model
        );
        virtual ~SkinnedModelActor() override;
        virtual void Update(float deltaTime) override;

    public:
        SkinnedModelRenderer* GetSkinnedModelRenderer() const;
        AnimComponent* GetAnimComponent() const;

    private:
        AnimComponentUPtr m_animComponent;
    };
}