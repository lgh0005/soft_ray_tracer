#pragma once
#include "Enums.h"

namespace bdt
{
	BDT_CLASS_PTR(RootComponent)

	class Actor
	{
	public:
		Actor(const String& tag);
		virtual ~Actor();
		virtual void Update(float deltaTime);

		const String& GetTag() const;
		uint64 GetActorID() const;
		void SetActorID(uint64 actorID);
		virtual EActorType GetActorType() const = 0;

	public:
		vec3 GetPosition() const;
		quat GetRotation() const;
		vec3 GetScale() const;
		mat4 GetTransformMatrix() const;

	protected:
		RootComponentUPtr m_rootComponent { nullptr };
		String m_tag;
		uint32 m_actorID {0xFFFFFFFF}; 
	};
}
