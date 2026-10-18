#pragma once

namespace bdt
{
	class Pose
	{
	public:
		explicit Pose
		(
			const vec3& position, 
			const quat& rotation, 
			const vec3& scale
		);
		~Pose();

		explicit operator mat4() const;
		static Pose Interpolate
		(
			const Pose& poseA,
			const Pose& poseB,
			float factor
		);

		const vec3& GetPosition() const { return m_position; }
		const quat& GetRotation() const { return m_rotation; }
		const vec3& GetScale() const { return m_scale; }

	private:
		vec3 m_position{ 0.0f };
		quat m_rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		vec3 m_scale{ 1.0f };
	};
}

