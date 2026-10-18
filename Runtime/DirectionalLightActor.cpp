#include "pch.h"
#include "DirectionalLightActor.h"
#include "DirectionalLight.h"

namespace bdt
{
    DirectionalLightActor::DirectionalLightActor(const String& tag)
        : Super(tag)
    {
        m_rootComponent = std::make_unique<DirectionalLight>(this);
    }

    DirectionalLightActor::~DirectionalLightActor() = default;

    DirectionalLight* DirectionalLightActor::GetDirectionalLight() const
    {
        return static_cast<DirectionalLight*>(m_rootComponent.get());
    }
}