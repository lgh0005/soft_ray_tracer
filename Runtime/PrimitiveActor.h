#pragma once
#include "Actor.h"

namespace bdt
{
    BDT_CLASS_PTR(PrimitiveRenderer)

    class PrimitiveActor : public Actor
    {
        using Super = Actor;

    public:
        explicit PrimitiveActor(const String& tag);
        virtual ~PrimitiveActor() override;
        virtual EActorType GetActorType() const override;

    public:
        PrimitiveRenderer* GetPrimitiveRenderer() const;
    };
}