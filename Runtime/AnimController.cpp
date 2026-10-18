#include "pch.h"
#include "AnimController.h"
#include "AnimState.h"
#include <Resources/Animation.h>

namespace bdt
{
    AnimController::AnimController() = default;
    AnimController::~AnimController() = default;

    void AnimController::AddState
    (
        const String& name,
        const ResourceHandle<Animation>& animation,
        bool isLoop,
        float speed
    )
    {
        m_states.try_emplace(name, animation, isLoop, speed);
    }

    bool AnimController::Play(const String& name)
    {
        auto it = m_states.find(name);
        if (it == m_states.end())
            return false;

        m_currentState = &it->second;
        m_currentTime = 0.0;

        m_nextState = nullptr;
        m_nextTime = 0.0;

        m_blendDuration = 0.0f;
        m_blendTime = 0.0f;

        m_isPlaying = true;
        m_isBlending = false;

        return true;
    }

    bool AnimController::CrossFade(const String& name, float duration)
    {
        auto it = m_states.find(name);
        if (it == m_states.end())
            return false;

        AnimState* target = &it->second;

        // 최초 재생 또는 즉시 전환
        if (!m_currentState || !m_isPlaying || duration <= 0.0f)
            return Play(name);

        // 동일한 상태로 중복 전환 방지
        if (m_isBlending && target == m_nextState)
            return true;

        if (!m_isBlending && target == m_currentState)
            return true;

        // 다음 애니메이션이 준비되지 않았다면 전환 보류
        if (!target->GetAnimation().Get())
            return false;

        m_nextState = target;
        m_nextTime = 0.0;

        m_blendDuration = duration;
        m_blendTime = 0.0f;

        m_isBlending = true;

        return true;
    }

    bool AnimController::GetPose
    (
        const String& nodeName, 
        const Pose& defaultPose,
        Pose& outputPose
    ) const
    {
        Pose currentPose = defaultPose;
        bool hasCurrentChannel = false;

        // 1. 현재 애니메이션 Pose 계산
        if (m_currentState)
        {
            AnimationPtr animation = m_currentState->GetAnimation().Get();
            if (animation)
            {
                const AnimChannelData* channel = animation->FindChannel(nodeName);
                if (channel) currentPose = animation->SamplePose(*channel, m_currentTime, defaultPose);
                hasCurrentChannel = true;
            }
        }

        // 2. CrossFade 중이 아니라면 현재 Pose 반환
        if (!m_isBlending || !m_nextState)
        {
            if (!hasCurrentChannel) return false;
            outputPose = currentPose;
            return true;
        }

        // 3. 다음 애니메이션 Pose 계산
        Pose nextPose = defaultPose;
        bool hasNextChannel = false;

        AnimationPtr nextAnimation = m_nextState->GetAnimation().Get();
        if (nextAnimation)
        {
            const AnimChannelData* channel = nextAnimation->FindChannel(nodeName);
            if (channel)
            {
                nextPose = nextAnimation->SamplePose(*channel, m_nextTime, defaultPose);
                hasNextChannel = true;
            }
        }

        // 4. 양쪽 모두 채널이 없으면 기본 행렬 사용
        if (!hasCurrentChannel && !hasNextChannel)
            return false;

        // 5. 현재 Pose와 다음 Pose 블렌딩
        outputPose = Pose::Interpolate(currentPose, nextPose, GetBlendFactor());
        return true;
    }

    void AnimController::Stop()
    {
        m_isPlaying = false;
        m_isBlending = false;

        m_nextState = nullptr;
        m_nextTime = 0.0;

        m_blendDuration = 0.0f;
        m_blendTime = 0.0f;
    }

    bool AnimController::UpdateStateTime
    (
        const AnimState* state,
        double& time,
        float deltaTime
    )
    {
        if (!state) return false;

        AnimationPtr animation = state->GetAnimation().Get();
        if (!animation) return false;

        const double duration = animation->GetDuration();
        const double tps = animation->GetTicksPerSecond();
        if (duration <= 0.0 || tps <= 0.0) return false;

        time += static_cast<double>(deltaTime) * state->GetSpeed() * tps;

        if (state->IsLoop())
        {
            time = std::fmod(time, duration);
            if (time < 0.0) time += duration;
        }
        else
        {
            time = glm::clamp(time, 0.0, duration);
        }

        return true;
    }

    void AnimController::Update(float deltaTime)
    {
        // 애니메이션 업데이트 상태 체크
        if (!m_isPlaying || !m_currentState)
            return;

        // 현재 애니메이션 시간 갱신
        if (!UpdateStateTime(m_currentState, m_currentTime, deltaTime))
            return;

        // CrossFade 중이 아니라면 재생 완료 확인
        if (!m_isBlending)
        {
            if (!m_currentState->IsLoop())
            {
                AnimationPtr animation = m_currentState->GetAnimation().Get();
                const double duration = animation->GetDuration();
                const float speed = m_currentState->GetSpeed();

                if ((speed > 0.0f && m_currentTime >= duration) ||
                    (speed < 0.0f && m_currentTime <= 0.0))
                    m_isPlaying = false;
            }

            return;
        }

        // 다음 애니메이션 시간 갱신
        if (!m_nextState) { m_isBlending = false; return; }
        if (!UpdateStateTime(m_nextState, m_nextTime, deltaTime))
            return;

        // CrossFade 진행
        m_blendTime += deltaTime;
        if (m_blendTime >= m_blendDuration)
        {
            m_currentState = m_nextState;
            m_currentTime = m_nextTime;
            m_nextState = nullptr;
            m_nextTime = 0.0;
            m_blendDuration = 0.0f;
            m_blendTime = 0.0f;
            m_isBlending = false;
        }
    }

    const AnimState* AnimController::GetCurrentState() const
    {
        return m_currentState;
    }

    const AnimState* AnimController::GetNextState() const
    {
        return m_nextState;
    }

    double AnimController::GetPlaybackTime() const
    {
        return m_currentTime;
    }

    double AnimController::GetNextPlaybackTime() const
    {
        return m_nextTime;
    }

    float AnimController::GetBlendFactor() const
    {
        if (!m_isBlending || m_blendDuration <= 0.0f) return 0.0f;
        return glm::clamp(m_blendTime / m_blendDuration, 0.0f, 1.0f);
    }

    bool AnimController::IsBlending() const
    {
        return m_isBlending;
    }

    bool AnimController::IsPlaying() const
    {
        return m_isPlaying;
    }
}
