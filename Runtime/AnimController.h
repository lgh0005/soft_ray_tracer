#pragma once

namespace bdt
{
	BDT_CLASS_PTR(Animation)
	BDT_CLASS_PTR(AnimState)

	class AnimController
	{
	public:
		AnimController();
		~AnimController();

	public:
		void AddState
		(
			const String& name,
			const ResourceHandle<Animation>& animation,
			bool isLoop = true,
			float speed = 1.0f
		);

		bool Play(const String& name);
		bool CrossFade(const String& name, float duration);
		bool GetPose(const String& nodeName, const Pose& defaultPose, Pose& outputPose) const;

		void Stop();
		void Update(float deltaTime);

		const AnimState* GetCurrentState() const;
		const AnimState* GetNextState() const;

		double GetPlaybackTime() const;
		double GetNextPlaybackTime() const;

		float GetBlendFactor() const;
		bool IsBlending() const;
		bool IsPlaying() const;

	private:
		bool UpdateStateTime
		(
			const AnimState* state,
			double& time,
			float deltaTime
		);

	private:
		Dictionary<String, AnimState> m_states;
		AnimState* m_currentState{ nullptr };
		AnimState* m_nextState{ nullptr };

		double m_currentTime{ 0.0 };
		double m_nextTime{ 0.0 };
		float m_blendDuration{ 0.0f };
		float m_blendTime{ 0.0f };

		bool m_isPlaying{ false };
		bool m_isBlending{ false };
	};
}
