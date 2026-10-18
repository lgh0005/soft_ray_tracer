#pragma once
#include "LightActor.h"

namespace bdt
{
    BDT_CLASS_PTR(SpotLight)

    class SpotLightActor : public LightActor
    {
        using Super = LightActor;

    public:
        explicit SpotLightActor(const String& tag);
        virtual ~SpotLightActor() override;

    public:
        SpotLight* GetSpotLight() const;
    };
}