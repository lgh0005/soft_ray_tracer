#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD430_BASE_ALIGNMENT) StaticInstanceProperty
	{
		mat4   worldMatrix { 1.0f };
		uint32 objectID    { 0 };
		vec3   pad0        { 0 };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) SkinnedInstanceProperty
	{
		mat4   worldMatrix { 1.0f };
		uint32 objectID    { 0 };
		uint32 boneOffset  { 0 };
		uint32 boneCount   { 0 };
		uint32 pad0        { 0 };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) LightData
	{
		// 위치와 방향은 월드 공간 기준이다. direction.xyz는 단위 벡터다.
		vec4 position{ 0.0f, 0.0f, 0.0f, 1.0f };
		vec4 direction{ 0.0f, -1.0f, 0.0f, 0.0f };
		vec4 color{ 1.0f, 1.0f, 1.0f, 0.0f }; // xyz: 선형 RGB, w: 미사용

		float intensity{ 1.0f };
		float range{ 10.0f }; // Point/Spot의 월드 공간 영향 거리
		float innerConeAngle{ 0.976296f }; // Spot 내부 반각 12.5도의 cos
		float outerConeAngle{ 0.953717f }; // Spot 외부 반각 17.5도의 cos

		uint32 type{ 0 }; // GLSL uint: 0=Directional, 1=Point, 2=Spot
		float attenuationConstant{ 1.0f };
		float attenuationLinear{ 0.0f };
		float attenuationQuadratic{ 0.0f };
	};
}