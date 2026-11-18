#pragma once
#include <Core/Types.h>
#include "Enums.h"

namespace bdt
{
	struct RTBVHMeshInput
	{
		uint32 meshIndex{ 0 };
		const List<vec3>* vertices{ nullptr };
		const List<uint32>* indices{ nullptr };
	};

	struct RTBVHBuildSettings
	{
		EBVHBuildMode blasBuildMode{ EBVHBuildMode::SAH };
		EBVHBuildMode tlasBuildMode{ EBVHBuildMode::SAH };
		uint32 blasMaxLeafPrimitives{ 4 };
		uint32 tlasMaxLeafPrimitives{ 1 };
	};
}
