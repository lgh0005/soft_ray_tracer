#include "pch.h"
#include "PointLightActor.h"
#include "PointLight.h"

namespace bdt
{
    PointLightActor::PointLightActor(const String& tag)
        : Super(tag)
    {
        m_rootComponent = std::make_unique<PointLight>(this);
    }

    PointLightActor::~PointLightActor() = default;

    PointLight* PointLightActor::GetPointLight() const
    {
        return static_cast<PointLight*>(m_rootComponent.get());
    }
}