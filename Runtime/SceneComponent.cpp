#include "pch.h"
#include "SceneComponent.h"

namespace bdt
{
    SceneComponent::SceneComponent(Actor* owner) : m_owner(owner) { }
    SceneComponent::~SceneComponent() = default;

    Actor* SceneComponent::GetOwner() const
    {
        return m_owner;
    }
}