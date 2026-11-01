#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPURay
	{
		vec4 origin    { 0.0f };
		vec4 direction { 0.0f };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPURayPayload
	{
		vec4 throughput   { 1.0f };   // rgb: 광선 경로의 기여도
		uint32 pixelIndex { 0 };      // 최종 결과가 기록될 픽셀
		uint32 depth	  { 0 };      // 현재 Bounce 깊이
		uint32 flags	  { 0 };      // 향후 Ray 유형
		uint32 pad0		  { 0 };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPURayWorkItem
	{
		GPURay ray{};
		GPURayPayload payload{};
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUHit
	{
		vec4 intersection{ 0.0f };  // x: 교차 거리, y: Barycentric u, z: Barycentric v, w: 미사용
		uvec4 identifiers{ 0 }; // x: Instance ID, y: Primitive ID, z: Material ID, w: 미사용
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) RTSkinningJob
	{
		uint32 sourceVertexOffset{ 0 };
		uint32 outputVertexOffset{ 0 };
		uint32 vertexCount{ 0 };
		uint32 boneOffset{ 0 };
		uint32 boneCount{ 0 };
	};
}