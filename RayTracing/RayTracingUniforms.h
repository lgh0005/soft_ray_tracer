#pragma once
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD140_BASE_ALIGNMENT) RayTracingSettings
	{
		uint32 maxBounces{ 3 };
		uint32 rayStackCapacity{ 4 };
		bool shadows{ true };
		bool environment{ true };
		float exposure{ 1.0f };
	};
}