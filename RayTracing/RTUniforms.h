#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
    inline constexpr uint32 RTMaxBounces = 3;
    static_assert(RTMaxBounces < 32);

	struct alignas(BDT_STD140_BASE_ALIGNMENT) RTFrameData
	{
		uint32 rayCount{ 0 };
		uint32 instanceCount{ 0 };
		uint32 pad0{ 0 };
		uint32 pad1{ 0 };
	};
}