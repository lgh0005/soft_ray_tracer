#include "pch.h"
#include "AnimState.h"

namespace bdt
{
    AnimState::AnimState
    (
        const ResourceHandle<Animation>& animation,
        bool isLoop,
        float speed
    )
        : m_animation(animation)
        , m_isLoop(isLoop)
        , m_speed(speed) { }

    AnimState::~AnimState() = default;

    const ResourceHandle<Animation>& AnimState::GetAnimation() const
    {
        return m_animation;
    }

    bool AnimState::IsLoop() const
    {
        return m_isLoop;
    }

    float AnimState::GetSpeed() const
    {
        return m_speed;
    }

    void AnimState::SetLoop(bool isLoop)
    {
        m_isLoop = isLoop;
    }

    void AnimState::SetSpeed(float speed)
    {
        m_speed = speed;
    }
}