#pragma once

namespace bdt
{
	BDT_CLASS_PTR(Actor)

	class SceneComponent
	{

	public:
		explicit SceneComponent(Actor* owner);
		virtual ~SceneComponent();
		Actor* GetOwner() const;

	protected:
		Actor* m_owner;
	};
}

