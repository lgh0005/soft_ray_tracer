#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD140_BASE_ALIGNMENT) RTFrameData
	{
		uint32 rayCount{ 0 };
		uint32 instanceCount{ 0 };
		uint32 pad0{ 0 };
		uint32 pad1{ 0 };
	};
}