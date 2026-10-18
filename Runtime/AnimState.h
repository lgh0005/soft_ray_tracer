#pragma once
#include <Resources/Animation.h>

namespace bdt
{
	class AnimState
	{
    public:
        explicit AnimState
        (
            const ResourceHandle<Animation>& animation,
            bool isLoop = true,
            float speed = 1.0f
        );
        ~AnimState();

    public:
        const ResourceHandle<Animation>& GetAnimation() const;
        bool IsLoop() const;
        float GetSpeed() const;

        void SetLoop(bool isLoop);
        void SetSpeed(float speed);

    private:
        ResourceHandle<Animation> m_animation;
        bool m_isLoop{ true };
        float m_speed{ 1.0f };
	};
}
