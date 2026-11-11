#include "pch.h"
#include "TLAS.h"
#include <cmath>

namespace bdt
{
    TLAS::TLAS(EBVHBuildMode buildMode) : Super(buildMode) { }
    TLAS::~TLAS() = default;

    bool TLAS::Build
    (
        const List<GPUBVHNodeBounds>& instanceBounds,
        uint32 maxLeafPrimitives
    )
    {
        Clear();
        if (!ValidateInstanceBounds(instanceBounds)) return false;
        return BuildFromBounds(instanceBounds, maxLeafPrimitives);
    }

    bool TLAS::Rebuild(const List<GPUBVHNodeBounds>& instanceBounds, uint32 maxLeafPrimitives)
    {
        return Build(instanceBounds, maxLeafPrimitives);
    }

    bool TLAS::Refit(const List<GPUBVHNodeBounds>& instanceBounds)
    {
        if (IsEmpty()) return false;
        if (instanceBounds.size() != GetPrimitiveCount()) return false;
        if (!ValidateInstanceBounds(instanceBounds)) return false;

        const auto& topology = GetTopology();
        const auto& bounds = GetBounds();
        const auto& primitiveIndices = GetPrimitiveIndices();

        // Primitive 인덱스와 Topology를 유지하며 Bounds만 갱신한다.
        for (uint32 nodeIndex = GetNodeCount(); nodeIndex-- > 0;)
        {
            const GPUBVHNodeTopology& node = topology[nodeIndex];
            GPUBVHNodeBounds updated{};
            if (IsLeaf(nodeIndex))
            {
                vec3 minBounds(MaxValue<float>);
                vec3 maxBounds(MinValue<float>);
                for (uint32 i = 0; i < node.primitiveRange.y; ++i)
                {
                    const uint32 primitiveIndex = primitiveIndices[node.primitiveRange.x + i];
                    const GPUBVHNodeBounds& primitive = instanceBounds[primitiveIndex];
                    minBounds = glm::min(minBounds, vec3(primitive.aabbMin));
                    maxBounds = glm::max(maxBounds, vec3(primitive.aabbMax));
                }
                updated.aabbMin = vec4(minBounds, 0.0f);
                updated.aabbMax = vec4(maxBounds, 0.0f);
            }
            else
            {
                const GPUBVHNodeBounds& left = bounds[node.treeTopology.x];
                const GPUBVHNodeBounds& right = bounds[node.treeTopology.y];
                updated.aabbMin = vec4(glm::min(vec3(left.aabbMin), vec3(right.aabbMin)), 0.0f);
                updated.aabbMax = vec4(glm::max(vec3(left.aabbMax), vec3(right.aabbMax)), 0.0f);
            }

            UpdateNodeBounds(nodeIndex, updated);
        }
        return true;
    }

    bool TLAS::ValidateInstanceBounds(const List<GPUBVHNodeBounds>& instanceBounds) const
    {
        if (instanceBounds.size() > MaxValue<uint32>) return false;
        for (const GPUBVHNodeBounds& bounds : instanceBounds)
        {
            const vec3 bmin(bounds.aabbMin);
            const vec3 bmax(bounds.aabbMax);
            if (glm::any(glm::greaterThan(bmin, bmax)))
                return false;
        }

        return true;
    }
    
}
