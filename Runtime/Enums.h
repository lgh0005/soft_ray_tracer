#pragma once
#include <Core/Types.h>

namespace bdt
{
	enum class EPrimitiveType : uint8
	{
		Box,
		Sphere
	};

	enum class EActorType : uint8
	{
		Camera,
		Light,
		Primitive,
		Model
	};
}
