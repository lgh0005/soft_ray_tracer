#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
	// GLSL의 layout(std140) 블록도 아래와 같은 타입과 순서로 선언한다.
	// vec4의 미사용 w는 패딩이다. 텍스처 핸들은 별도로 바인딩한다.
	struct alignas(BDT_STD140_BASE_ALIGNMENT) MaterialData
	{
		vec4 albedoFactor      { 1.0f };
		vec4 emissiveFactor    { 0.0f }; // xyz: 발광 색상, w: 미사용
		float metallicFactor   { 0.0f };
		float roughnessFactor  { 1.0f };
		float emissionStrength { 1.0f };
		float heightScale      { 0.0f };
		uint32 textureFlags    { 0 };
		uint32 pad0[3]		   { 0 };
	};

	struct alignas(BDT_STD140_BASE_ALIGNMENT) CameraData
	{
		// GLSL 기본 column-major 기준. viewProjection = projection * view.
		mat4 view           { 1.0f };
		mat4 projection     { 1.0f };
		mat4 viewProjection { 1.0f };
		vec4 position       { 0.0f, 0.0f, 0.0f, 1.0f }; // 월드 공간 카메라 위치

		float nearPlane     { 0.1f };
		float farPlane      { 1000.0f };
		vec2 viewportSize   { 1.0f, 1.0f }; // 프레임버퍼 픽셀 크기
	};
}
