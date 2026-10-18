#include "pch.h"
#include "LightActor.h"
#include "LightComponent.h"

namespace bdt
{
    LightActor::LightActor(const String& tag) : Super(tag) { }
    LightActor::~LightActor() = default;

    LightComponent* LightActor::GetLightComponent() const
    {
        return static_cast<LightComponent*>(m_rootComponent.get());
    }

    EActorType LightActor::GetActorType() const
    {
        return EActorType::Light;
    }
}
