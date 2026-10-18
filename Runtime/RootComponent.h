#pragma once
#include "SceneComponent.h"

namespace bdt
{
	BDT_CLASS_PTR(Actor)

	class RootComponent : public SceneComponent
	{
		using Super = SceneComponent;

	public:
		RootComponent(Actor* owner);
		virtual ~RootComponent();
		using Super = SceneComponent;

	public:
		const vec3& GetPosition() const;
		const quat& GetRotation() const;
		const vec3& GetScale() const;

		void SetPosition(const vec3& position);
		void SetRotation(const quat& rotation);
		void SetScale(const vec3& scale);

		// 오일러 각도는 (X, Y, Z) 축 기준이며 단위는 라디안이다.
		vec3 GetEulerAngles() const;
		void SetEulerAngles(const vec3& radians);

		// 이동은 월드 축 기준, 추가 회전은 Component의 로컬 축 기준이다.
		void Translate(const vec3& offset);
		void Rotate(const quat& deltaRotation);

		// 로컬 좌표를 월드 좌표로 변환하는 T * R * S 행렬.
		mat4 GetTransformMatrix() const;
		bool IsTransformDirty() const;
		virtual void MarkTransformDirty();

	private:
		vec3 m_position{ 0.0f };
		quat m_rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		vec3 m_scale{ 1.0f };

		// World Space Transform Properties
		mutable mat4 m_transformMatrix { 1.0f };
		mutable bool m_transformDirty{ true };
	};
}
