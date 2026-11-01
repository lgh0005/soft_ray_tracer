#pragma once
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPURay
	{
		vec4 originDepth{ 0 };
		vec4 direction{ 0 };
		vec4 weight{ 0 };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPURayState
	{
		GPURay current{};
		GPURay secondary{};
		vec4   radiance{ 0 };
		uvec4  control{ 0 };
	};
}