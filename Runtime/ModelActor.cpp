#include "pch.h"
#include "ModelActor.h"
#include "ModelRenderer.h"

namespace bdt
{
    ModelActor::ModelActor(const String& tag) : Super(tag) {}
    ModelActor::~ModelActor() = default;

    ModelRenderer* ModelActor::GetModelRenderer() const
    {
        return static_cast<ModelRenderer*>(m_rootComponent.get());
    }

    EActorType ModelActor::GetActorType() const
    {
        return EActorType::Model;
    }
}