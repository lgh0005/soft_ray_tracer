#include "pch.h"
#include "Actor.h"
#include "RootComponent.h"

namespace bdt
{
	Actor::Actor(const String& tag)
		:  m_tag(tag) { }

	Actor::~Actor() = default;

	const String& Actor::GetTag() const
	{
		return m_tag;
	}

    uint64 Actor::GetActorID() const
    {
        return m_actorID;
    }

    void Actor::SetActorID(uint64 actorID)
    {
        m_actorID = actorID;
    }

    void Actor::Update(float deltaTime)
    {
        // 모든 Actor가 공통으로 사용하는 프레임 갱신 함수.
        // 기본 Actor는 별도의 갱신 작업을 수행하지 않으며,
        // 필요한 경우 파생 클래스에서 재정의하여 고유한 동작을 처리한다.
    }

    vec3 Actor::GetPosition() const
    {
        return m_rootComponent->GetPosition();
    }

    quat Actor::GetRotation() const
    {
        return m_rootComponent->GetRotation();
    }

    vec3 Actor::GetScale() const
    {
        return m_rootComponent->GetScale();
    }

    mat4 Actor::GetTransformMatrix() const
    {
        return m_rootComponent->GetTransformMatrix();
    }
}
