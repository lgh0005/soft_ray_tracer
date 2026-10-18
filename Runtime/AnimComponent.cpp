
#include "pch.h"
#include "AnimComponent.h"

namespace bdt
{
    AnimComponent::AnimComponent(Actor* owner)
        : Super(owner) {}

    AnimComponent::~AnimComponent() = default;

    void AnimComponent::Update(float deltaTime)
    {
        m_controller.Update(deltaTime);
    }

    AnimController& AnimComponent::GetController()
    {
        return m_controller;
    }

    const AnimController& AnimComponent::GetController() const
    {
        return m_controller;
    }

    bool AnimComponent::EvaluatePose
    (
        const Model& model, 
        List<mat4>& outputGlobalTransforms
    ) const
    {
        const auto& nodes = model.GetData().nodes;
        const AnimController& controller = GetController();

        // 기존 버퍼 재사용
        outputGlobalTransforms.resize(nodes.size());
        for (usize i = 0; i < nodes.size(); ++i)
        {
            const ModelNode& node = nodes[i];
            mat4 localTransform = node.localTransform;
            Pose pose = node.localPose;

            // 애니메이션 채널이 존재하는 경우에만 행렬 계산
            if (controller.GetPose(node.name, node.localPose, pose))
                localTransform = static_cast<mat4>(pose);

            if (node.parentIndex < 0)
            {
                outputGlobalTransforms[i] = localTransform;
            }
            else
            {
                const usize parentIndex = static_cast<usize>(node.parentIndex);
                if (parentIndex >= i) return false;
                outputGlobalTransforms[i] = outputGlobalTransforms[parentIndex] * localTransform;
            }
        }

        return true;
    }
}
