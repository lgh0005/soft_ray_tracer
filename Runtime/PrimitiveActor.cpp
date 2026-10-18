#include "pch.h"
#include "PrimitiveActor.h"
#include "PrimitiveRenderer.h"

namespace bdt
{
    PrimitiveActor::PrimitiveActor(const String& tag) : Super(tag)
    {
        m_rootComponent = std::make_unique<PrimitiveRenderer>(this);
    }

    PrimitiveActor::~PrimitiveActor() = default;

    EActorType PrimitiveActor::GetActorType() const
    {
        return EActorType::Primitive;
    }

    PrimitiveRenderer* PrimitiveActor::GetPrimitiveRenderer() const
    {
        return static_cast<PrimitiveRenderer*>(m_rootComponent.get());
    }
}
