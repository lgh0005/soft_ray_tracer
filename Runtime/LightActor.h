#pragma once
#include "Actor.h"

namespace bdt
{
    BDT_CLASS_PTR(LightComponent)

    class LightActor : public Actor
    {
        using Super = Actor;

    public:
        explicit LightActor(const String& tag);
        virtual ~LightActor() override;
        virtual EActorType GetActorType() const override;

    public:
        LightComponent* GetLightComponent() const;
    };
}