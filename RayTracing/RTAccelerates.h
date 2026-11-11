#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUBVHNodeTopology
	{
		// x: leftChild, y: rightChild
		// z: parentIndex, w: isLeaf
		uvec4 treeTopology{ 0 };

		// x: firstPrimitive, y: primitiveCount
		// z: reserved, w: escapeIndex
		uvec4 primitiveRange{ 0 };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUBVHNodeBounds
	{
		vec4 aabbMin{ 0.0f };
		vec4 aabbMax{ 0.0f };
	};

	struct alignas(BDT_STD430_BASE_ALIGNMENT) GPUBVHDescriptor
	{
		// x: nodeOffset, y: nodeCount, 
		// z: primitiveOffset, w: primitiveCount
		uvec4 ranges{ 0u };

		// x: absolute root node, y: meshIndex (INVALID for TLAS), 
		// z/w: reserved
		uvec4 metadata{ 0u };
	};
}
