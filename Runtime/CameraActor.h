#pragma once
#include "Pawn.h"

namespace bdt
{
	BDT_CLASS_PTR(CameraComponent)

	class CameraActor : public Pawn
	{
		using Super = Pawn;

	public:
		explicit CameraActor(const String& tag);
		virtual ~CameraActor() override;
		virtual EActorType GetActorType() const override;

	public:
		CameraComponent* GetCameraComponent() const;
	};
}
