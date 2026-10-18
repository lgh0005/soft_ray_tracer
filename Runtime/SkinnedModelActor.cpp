#include "pch.h"
#include "SkinnedModelActor.h"
#include "SkinnedModelRenderer.h"
#include "CharacterMovement.h"
#include "AnimComponent.h"

namespace bdt
{
    SkinnedModelActor::SkinnedModelActor
    (
        const String& tag,
        const ResourceHandle<Model>& model
    ) : Super(tag)
    {
        // 스키닝 모델 렌더러 생성
        m_rootComponent = std::make_unique<SkinnedModelRenderer>(this, model);

        // 애니메이션 컴포넌트 생성
        m_animComponent = std::make_unique<AnimComponent>(this);

        // 캐릭터 이동 컴포넌트 생성
        m_movementComponent = std::make_unique<CharacterMovement>(this, m_rootComponent.get());
    }

    SkinnedModelActor::~SkinnedModelActor() = default;

    void SkinnedModelActor::Update(float deltaTime)
    {
        Super::Update(deltaTime);
        
        if (!m_animComponent) return;

        // 1. 애니메이션 재생 시간 갱신
        m_animComponent->Update(deltaTime);

        // 2. 렌더러 확인
        auto modelRenderer = static_cast<SkinnedModelRenderer*>(GetModelRenderer());
        if (!modelRenderer) return;

        // 3. 모델 리소스 확인
        ModelPtr model = modelRenderer->GetModel();
        if (!model) return;

        // 4. 현재 애니메이션 포즈 평가
        List<mat4> nodeGlobalTransforms;
        if (!m_animComponent->EvaluatePose(*model, nodeGlobalTransforms))
            return;

        // 5. Bone Palette 갱신
        modelRenderer->ApplyPose(nodeGlobalTransforms);
    }

    SkinnedModelRenderer* SkinnedModelActor::GetSkinnedModelRenderer() const
    {
        return static_cast<SkinnedModelRenderer*>(m_rootComponent.get());
    }

    AnimComponent* SkinnedModelActor::GetAnimComponent() const
    {
        return m_animComponent.get();
    }
}
