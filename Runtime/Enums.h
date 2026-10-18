#pragma once
#include <Core/Types.h>

namespace bdt
{
	enum class EActorType : uint8
	{
		Camera,
		Light,
		Primitive,
		Model
	};

	enum class ELightType : uint8
	{
		Directional,
		Point,
		Spot
	};
}