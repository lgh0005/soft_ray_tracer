#pragma once
#include "LightActor.h"

namespace bdt
{
    BDT_CLASS_PTR(PointLight)

    class PointLightActor : public LightActor
    {
        using Super = LightActor;

    public:
        explicit PointLightActor(const String& tag);
        virtual ~PointLightActor() override;

    public:
        PointLight* GetPointLight() const;
    };
}