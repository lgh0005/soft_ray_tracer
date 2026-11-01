#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUStaticVertex
	{
		vec4 position{ 0.0f };
		vec4 normal{ 0.0f };
		vec4 uv{ 0.0f };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUSkinnedVertex
	{
		vec4 position{ 0.0f };
		vec4 normal{ 0.0f };
		vec4 tangent{ 0.0f };
		vec4 uv{ 0.0f };
		uvec4 boneIndices{ 0 };
		vec4 boneWeights{ 0.0f };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUMeshDescriptor
	{
		uint32 vertexOffset{ 0 };    // 전체 Vertex Buffer에서 이 메시의 시작 인덱스
		uint32 vertexCount{ 0 };     // 메시의 정점 개수
		uint32 triangleOffset{ 0 };  // 전체 Triangle Buffer에서 이 메시의 시작 인덱스
		uint32 triangleCount{ 0 };   // 메시의 삼각형 개수
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUInstance
	{
		mat4 worldMatrix{ 1.0f };
		mat4 inverseWorldMatrix{ 1.0f };
		uint32 meshIndex{ 0 };
		uint32 materialIndex{ 0 };
		uint32 boneOffset{ 0 };
		uint32 boneCount{ 0 };

		// GPU Skinning
		uint32 skinnedVertexOffset;
		uint32 pad0;
		uint32 pad1;
		uint32 pad2;
	};
}