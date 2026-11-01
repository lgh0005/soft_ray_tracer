#pragma once
#include "Actor.h"
#include <Resources/ResourceHandle.h>

namespace bdt
{
    BDT_CLASS_PTR(PrimitiveRenderer)
    BDT_CLASS_PTR(Material)

    class PrimitiveActor : public Actor
    {
        using Super = Actor;

    public:
        explicit PrimitiveActor
        (
            const String& tag,
            EPrimitiveType type = EPrimitiveType::Box,
            const ResourceHandle<Material>& material = {}
        );
        virtual ~PrimitiveActor() override;
        virtual EActorType GetActorType() const override;

    public:
        PrimitiveRenderer* GetPrimitiveRenderer() const;
    };
}
