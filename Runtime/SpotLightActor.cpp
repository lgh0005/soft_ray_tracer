#include "pch.h"
#include "SpotLightActor.h"
#include "SpotLight.h"

namespace bdt
{
    SpotLightActor::SpotLightActor(const String& tag)
        : Super(tag)
    {
        m_rootComponent = std::make_unique<SpotLight>(this);
    }

    SpotLightActor::~SpotLightActor() = default;

    SpotLight* SpotLightActor::GetSpotLight() const
    {
        return static_cast<SpotLight*>(m_rootComponent.get());
    }
}