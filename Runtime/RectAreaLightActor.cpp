#include "pch.h"
#include "RectAreaLightActor.h"
#include "RectAreaLight.h"

namespace bdt
{
    RectAreaLightActor::RectAreaLightActor(const String& tag)
        : Super(tag)
    {
        m_rootComponent = std::make_unique<RectAreaLight>(this);
    }

    RectAreaLightActor::~RectAreaLightActor() = default;

    RectAreaLight* RectAreaLightActor::GetRectAreaLight() const
    {
        return static_cast<RectAreaLight*>(m_rootComponent.get());
    }
}