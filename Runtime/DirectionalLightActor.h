#pragma once
#include "LightActor.h"

namespace bdt
{
    BDT_CLASS_PTR(DirectionalLight)

    class DirectionalLightActor : public LightActor
    {
        using Super = LightActor;

    public:
        explicit DirectionalLightActor(const String& tag);
        virtual ~DirectionalLightActor() override;

    public:
        DirectionalLight* GetDirectionalLight() const;
    };
}