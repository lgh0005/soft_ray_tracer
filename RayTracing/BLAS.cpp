#include "pch.h"
#include "BLAS.h"
#include <cmath>

namespace bdt
{
    BLAS::BLAS(uint32 meshIndex, EBVHBuildMode buildMode) 
        : Super(buildMode), m_meshIndex(meshIndex) { }
    BLAS::~BLAS() = default;

    bool BLAS::Build
    (
        const List<vec3>& vertices,
        const List<uint32>& indices,
        uint32 maxLeafPrimitives
    )
    {
        Clear();
        List<GPUBVHNodeBounds> triangleBounds;
        if (!CalculateTriangleBounds(vertices, indices, triangleBounds)) return false;
        return BuildFromBounds(triangleBounds, maxLeafPrimitives);
    }

    bool BLAS::Rebuild
    (
        const List<vec3>& vertices,
        const List<uint32>& indices,
        uint32 maxLeafPrimitives
    )
    {
        return Build(vertices, indices, maxLeafPrimitives);
    }

    bool BLAS::Refit(const List<vec3>& vertices, const List<uint32>& indices)
    {
        if (IsEmpty()) return false;

        List<GPUBVHNodeBounds> triangleBounds;
        if (!CalculateTriangleBounds(vertices, indices, triangleBounds)) return false;
        if (triangleBounds.size() != GetPrimitiveCount()) return false;

        const auto& topology = GetTopology();
        const auto& bounds = GetBounds();
        const auto& primitiveIndices = GetPrimitiveIndices();

        // DFS 전위 배치의 역순으로 자식을 먼저 갱신하고 부모 경계를 병합한다.
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
                    const GPUBVHNodeBounds& primitive = triangleBounds[primitiveIndex];
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

    bool BLAS::CalculateTriangleBounds
    (
        const List<vec3>& vertices,
        const List<uint32>& indices,
        List<GPUBVHNodeBounds>& triangleBounds
    ) const
    {
        triangleBounds.clear();
        if (indices.size() % 3 != 0) return false;
        if (indices.size() / 3 > MaxValue<uint32>) return false;
        const uint32 triangleCount = static_cast<uint32>(indices.size() / 3);
        triangleBounds.reserve(triangleCount);

        for (uint32 i = 0; i < triangleCount; ++i)
        {
            const usize indexOffset = static_cast<usize>(i) * 3;
            const uint32 i0 = indices[indexOffset + 0];
            const uint32 i1 = indices[indexOffset + 1];
            const uint32 i2 = indices[indexOffset + 2];

            if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
                return false;

            const vec3& v0 = vertices[i0];
            const vec3& v1 = vertices[i1];
            const vec3& v2 = vertices[i2];

            const vec3 minBounds = glm::min(glm::min(v0, v1), v2);
            const vec3 maxBounds = glm::max(glm::max(v0, v1), v2);

            GPUBVHNodeBounds bounds{};
            bounds.aabbMin = vec4(minBounds, 0.0f);
            bounds.aabbMax = vec4(maxBounds, 0.0f);

            triangleBounds.emplace_back(bounds);
        }

        return true;
    }

    uint32 BLAS::GetMeshIndex() const
    {
        return m_meshIndex;
    }
}
