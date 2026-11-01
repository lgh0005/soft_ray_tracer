#include "pch.h"
#include "PrimitiveActor.h"
#include "PrimitiveRenderer.h"
#include "BoxRenderer.h"
#include "SphereRenderer.h"

namespace bdt
{
    PrimitiveActor::PrimitiveActor(const String& tag, EPrimitiveType type, const ResourceHandle<Material>& material)
        : Super(tag)
    {
        switch (type)
        {
            case EPrimitiveType::Sphere:
                m_rootComponent = std::make_unique<SphereRenderer>(this, material);
                break;

            case EPrimitiveType::Box:
            default:
                m_rootComponent = std::make_unique<BoxRenderer>(this, material);
                break;
        }
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
