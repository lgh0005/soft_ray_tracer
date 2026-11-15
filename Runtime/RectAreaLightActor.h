#pragma once
#include "LightActor.h"

namespace bdt
{
    BDT_CLASS_PTR(RectAreaLight)

    class RectAreaLightActor : public LightActor
    {
        using Super = LightActor;

    public:
        explicit RectAreaLightActor(const String& tag);
        virtual ~RectAreaLightActor() override;

    public:
        RectAreaLight* GetRectAreaLight() const;
    };
}

