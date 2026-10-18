#include "pch.h"
#include "CameraActor.h"
#include "CameraComponent.h"
#include "FloatingMovement.h"

namespace bdt
{
    CameraActor::CameraActor(const String& tag) : Super(tag)
    {
        // 카메라 컴포넌트 생성
        m_rootComponent = std::make_unique<CameraComponent>(this);

        // 이동 컴포넌트 생성
        m_movementComponent = std::make_unique<FloatingMovement>(this, m_rootComponent.get());
    }

    CameraActor::~CameraActor() = default;

    CameraComponent* CameraActor::GetCameraComponent() const
    {
        return static_cast<CameraComponent*>(m_rootComponent.get());
    }

    EActorType CameraActor::GetActorType() const
    {
        return EActorType::Camera;
    }
}