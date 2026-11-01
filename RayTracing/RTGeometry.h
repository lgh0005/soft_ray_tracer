#pragma once
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUVertex
	{
		vec4 position{ 0 };
		vec4 normal{ 0 };
		vec4 tangent{ 0 };
		vec4 bitangent{ 0 };
		vec4 uv{ 0 };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUTriangle
	{
		// TODO : 정점 설정 및 머티리얼 인덱스 따위를 매핑 필요
	};
}