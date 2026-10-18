#include "pch.h"
#include "RootComponent.h"

namespace bdt
{
	RootComponent::RootComponent(Actor* owner) : Super(owner) { }
	RootComponent::~RootComponent() = default;

	const vec3& RootComponent::GetPosition() const
	{
		return m_position;
	}

	const quat& RootComponent::GetRotation() const
	{
		return m_rotation;
	}

	const vec3& RootComponent::GetScale() const
	{
		return m_scale;
	}

	void RootComponent::SetPosition(const vec3& position)
	{
		m_position = position;
		MarkTransformDirty();
	}

	void RootComponent::SetRotation(const quat& rotation)
	{
		m_rotation = glm::normalize(rotation);
		MarkTransformDirty();
	}

	void RootComponent::SetScale(const vec3& scale)
	{
		m_scale = scale;
		MarkTransformDirty();
	}

	vec3 RootComponent::GetEulerAngles() const
	{
		return glm::eulerAngles(m_rotation);
	}

	void RootComponent::SetEulerAngles(const vec3& radians)
	{
		SetRotation(quat(radians));
	}

	void RootComponent::Translate(const vec3& offset)
	{
		m_position += offset;
		MarkTransformDirty();
	}

	void RootComponent::Rotate(const quat& deltaRotation)
	{
		SetRotation(m_rotation * deltaRotation);
	}

	mat4 RootComponent::GetTransformMatrix() const
	{
		if (m_transformDirty)
		{
			m_transformMatrix = glm::mat4_cast(m_rotation);

			// Scale 적용
			m_transformMatrix[0] *= m_scale.x;
			m_transformMatrix[1] *= m_scale.y;
			m_transformMatrix[2] *= m_scale.z;

			// Translation 적용
			m_transformMatrix[3] = vec4(m_position, 1.0f);
			m_transformDirty = false;
		}

		return m_transformMatrix;
	}

	bool RootComponent::IsTransformDirty() const
	{
		return m_transformDirty;
	}

	void RootComponent::MarkTransformDirty()
	{
		m_transformDirty = true;
	}
}
