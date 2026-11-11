#include "pch.h"
#include "BVH.h"
#include <numeric>

namespace bdt
{
    BVH::BVH(EBVHBuildMode buildMode) : m_buildMode(buildMode) { }
	BVH::~BVH() = default;

    const List<GPUBVHNodeTopology>& BVH::GetTopology() const
    {
        return m_topology;
    }

    const List<GPUBVHNodeBounds>& BVH::GetBounds() const
    {
        return m_bounds;
    }

    const List<uint32>& BVH::GetPrimitiveIndices() const
    {
        return m_primitiveIndices;
    }

    uint32 BVH::GetRootIndex() const
    {
        return m_rootIndex;
    }

    uint32 BVH::GetNodeCount() const
    {
        return static_cast<uint32>(m_topology.size());
    }

    uint32 BVH::GetPrimitiveCount() const
    {
        return static_cast<uint32>(m_primitiveIndices.size());
    }

    bool BVH::IsEmpty() const
    {
        return m_rootIndex == 0xffffffffu;
    }

    bool BVH::IsLeaf(uint32 nodeIndex) const
    {
        if (nodeIndex >= m_topology.size()) return false;
        return m_topology[nodeIndex].treeTopology.w != 0u;
    }

    void BVH::UpdateNodeBounds(uint32 nodeIndex, const GPUBVHNodeBounds& bounds)
    {
        BDT_ASSERT(nodeIndex < m_bounds.size(), "Node index is out of bounds");
        m_bounds[nodeIndex] = bounds;
    }

    void BVH::Clear()
    {
        m_topology.clear();
        m_bounds.clear();
        m_primitiveIndices.clear();
        m_rootIndex = 0xffffffffu;
    }

    void BVH::SetData
    (
        List<GPUBVHNodeTopology>&& topology,
        List<GPUBVHNodeBounds>&& bounds,
        List<uint32>&& primitiveIndices,
        uint32 rootIndex
    )
    {
        m_topology = std::move(topology);
        m_bounds = std::move(bounds);
        m_primitiveIndices = std::move(primitiveIndices);
        m_rootIndex = rootIndex;
    }

    bool BVH::BuildFromBounds
    (
        const List<GPUBVHNodeBounds>& primitiveBounds,
        uint32 maxLeafPrimitives
    )
    {
        Clear();
        if (primitiveBounds.empty()) return true;
        if (maxLeafPrimitives == 0) return false;

        const uint32 primitiveCount = static_cast<uint32>(primitiveBounds.size());
        List<GPUBVHNodeTopology> topology;
        List<GPUBVHNodeBounds> bounds;
        List<uint32> indices(primitiveCount);

        std::iota(indices.begin(), indices.end(), 0u);
        topology.reserve(primitiveCount * 2);
        bounds.reserve(primitiveCount * 2);

        const uint32 rootIndex = BuildNode
        (
            primitiveBounds,
            topology,
            bounds,
            indices,
            0u,
            primitiveCount,
            0xffffffffu,
            maxLeafPrimitives
        );

        // Escape Index 보정
        const uint32 nodeCount = static_cast<uint32>(topology.size());
        for (auto& node : topology)
        {
            if (node.primitiveRange.w == nodeCount)
                node.primitiveRange.w = 0xffffffffu;
        }

        SetData
        (
            std::move(topology),
            std::move(bounds),
            std::move(indices),
            rootIndex
        );

        return true;
    }

    GPUBVHNodeBounds BVH::CalculateNodeBounds
    (
        const List<GPUBVHNodeBounds>& primitiveBounds,
        const List<uint32>& indices,
        uint32 begin,
        uint32 end
    ) const
    {
        vec3 minBounds(MaxValue<float>);
        vec3 maxBounds(MinValue<float>);
        for (uint32 i = begin; i < end; ++i)
        {
            const GPUBVHNodeBounds& primitive = primitiveBounds[indices[i]];
            minBounds = glm::min(minBounds, vec3(primitive.aabbMin));
            maxBounds = glm::max(maxBounds, vec3(primitive.aabbMax));
        }

        GPUBVHNodeBounds result{};
        result.aabbMin = vec4(minBounds, 0.0f);
        result.aabbMax = vec4(maxBounds, 0.0f);

        return result;
    }

    uint32 BVH::BuildNode
    (
        const List<GPUBVHNodeBounds>& primitiveBounds,
        List<GPUBVHNodeTopology>& topology,
        List<GPUBVHNodeBounds>& bounds,
        List<uint32>& indices,
        uint32 begin,
        uint32 end,
        uint32 parent,
        uint32 maxLeafPrimitives
    )
    {
        const uint32 nodeIndex = static_cast<uint32>(topology.size());
        topology.emplace_back();
        bounds.emplace_back();

        GPUBVHNodeTopology node{};
        node.treeTopology = uvec4(0xffffffffu, 0xffffffffu, parent, 0u);

        // 현재 Node의 AABB 계산
        const GPUBVHNodeBounds nodeBounds = CalculateNodeBounds(primitiveBounds, indices, begin, end);
        bounds[nodeIndex] = nodeBounds;
        const uint32 count = end - begin;
        uint32 splitIndex = begin;
        bool shouldSplit = false;

        // Leaf 최대 Primitive 개수를 초과한 경우 분할
        if (count > maxLeafPrimitives)
        {
            switch (m_buildMode)
            {
                case EBVHBuildMode::MedianSplit:
                {
                    shouldSplit = SplitMedian
                    (
                        primitiveBounds,
                        indices,
                        begin,
                        end,
                        splitIndex
                    );
                    break;
                }

                case EBVHBuildMode::SAH:
                {
                    shouldSplit = SplitSAH
                    (
                        primitiveBounds,
                        indices,
                        begin,
                        end,
                        nodeBounds,
                        splitIndex
                    );
                    break;
                }

                default:
                    BDT_ASSERT(false, "Unsupported BVH build mode");
                    break;
            }
        }

        // Leaf Node 생성
        if (!shouldSplit)
        {
            node.treeTopology.w = 1u;
            node.primitiveRange.x = begin;
            node.primitiveRange.y = count;
            node.primitiveRange.w = nodeIndex + 1u;
            topology[nodeIndex] = node;
            return nodeIndex;
        }

        // Internal Node - Left
        const uint32 left = BuildNode
        (
            primitiveBounds,
            topology,
            bounds,
            indices,
            begin,
            splitIndex,
            nodeIndex,
            maxLeafPrimitives
        );

        // Internal Node - Right
        const uint32 right = BuildNode
        (
            primitiveBounds,
            topology,
            bounds,
            indices,
            splitIndex,
            end,
            nodeIndex,
            maxLeafPrimitives
        );

        node.treeTopology.x = left;
        node.treeTopology.y = right;
        node.primitiveRange.w = static_cast<uint32>(topology.size());
        topology[nodeIndex] = node;

        return nodeIndex;
    }

    bool BVH::SplitMedian
    (
        const List<GPUBVHNodeBounds>& primitiveBounds,
        List<uint32>& indices,
        uint32 begin,
        uint32 end,
        uint32& splitIndex
    )
    {
        const uint32 count = end - begin;
        if (count <= 1) return false;

        // Centroid Bounds 계산
        vec3 centroidMin(MaxValue<float>);
        vec3 centroidMax(MinValue<float>);
        for (uint32 i = begin; i < end; ++i)
        {
            const GPUBVHNodeBounds& primitive = primitiveBounds[indices[i]];
            const vec3 centroid = (vec3(primitive.aabbMin) + vec3(primitive.aabbMax)) * 0.5f;
            centroidMin = glm::min(centroidMin, centroid);
            centroidMax = glm::max(centroidMax, centroid);
        }

        // 가장 긴 Centroid 축 선택
        const vec3 extent = centroidMax - centroidMin;
        uint32 axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;

        // 중앙값 위치
        splitIndex = begin + count / 2;

        // 중앙값 기준 분할
        std::nth_element
        (
            indices.begin() + begin,
            indices.begin() + splitIndex,
            indices.begin() + end,
            [&](uint32 a, uint32 b)
            {
                const GPUBVHNodeBounds& ba = primitiveBounds[a];
                const GPUBVHNodeBounds& bb = primitiveBounds[b];
                const float ca = (ba.aabbMin[axis] + ba.aabbMax[axis]) * 0.5f;
                const float cb = (bb.aabbMin[axis] + bb.aabbMax[axis]) * 0.5f;
                return ca < cb;
            }
        );

        return true;
    }

    float BVH::CalculateSurfaceArea(const GPUBVHNodeBounds& bounds) const
    {
        const vec3 extent = vec3(bounds.aabbMax) - vec3(bounds.aabbMin);
        return 2.0f * (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
    }

    bool BVH::SplitSAH
    (
        const List<GPUBVHNodeBounds>& primitiveBounds,
        List<uint32>& indices,
        uint32 begin,
        uint32 end,
        const GPUBVHNodeBounds& nodeBounds,
        uint32& splitIndex
    )
    {
        constexpr uint32 BIN_COUNT = 12;
        constexpr float TRAVERSAL_COST = 1.0f;
        constexpr float INTERSECTION_COST = 1.0f;
        constexpr float EPSILON = 1e-8f;
        struct Bin
        {
            vec3 minBounds{ MaxValue<float> };
            vec3 maxBounds{ MinValue<float> };
            uint32 count{ 0 };
        };

        const uint32 primitiveCount = end - begin;
        if (primitiveCount <= 1) return false;

        const float parentArea = CalculateSurfaceArea(nodeBounds);
        if (parentArea <= EPSILON) return false;

        // 1. Centroid Bounds 계산
        vec3 centroidMin(MaxValue<float>);
        vec3 centroidMax(MinValue<float>);

        for (uint32 i = begin; i < end; ++i)
        {
            const GPUBVHNodeBounds& primitive = primitiveBounds[indices[i]];
            const vec3 centroid = (vec3(primitive.aabbMin) + vec3(primitive.aabbMax)) * 0.5f;
            centroidMin = glm::min(centroidMin, centroid);
            centroidMax = glm::max(centroidMax, centroid);
        }

        const vec3 centroidExtent = centroidMax - centroidMin;

        // 2. Leaf 유지 비용
        const float leafCost = primitiveCount * INTERSECTION_COST;
        float bestCost = leafCost;

        uint32 bestAxis = 0;
        uint32 bestSplit = 0;
        bool found = false;

        // 3. X, Y, Z 축별 후보 평가
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            if (centroidExtent[axis] <= EPSILON)
                continue;

            Bin bins[BIN_COUNT];

            // Primitive를 Bin에 분배
            for (uint32 i = begin; i < end; ++i)
            {
                const GPUBVHNodeBounds& primitive = primitiveBounds[indices[i]];
                const float centroid = (primitive.aabbMin[axis] + primitive.aabbMax[axis]) * 0.5f;
                const float normalized = (centroid - centroidMin[axis]) / centroidExtent[axis];
                const uint32 binIndex = std::min(static_cast<uint32>(normalized * BIN_COUNT), BIN_COUNT - 1u);

                Bin& bin = bins[binIndex];
                bin.minBounds = glm::min(bin.minBounds, vec3(primitive.aabbMin));
                bin.maxBounds = glm::max(bin.maxBounds, vec3(primitive.aabbMax));
                ++bin.count;
            }

            // 4. Prefix / Suffix 계산
            Bin prefix[BIN_COUNT];
            Bin suffix[BIN_COUNT];
            Bin accumulated{};
            for (uint32 i = 0; i < BIN_COUNT; ++i)
            {
                if (bins[i].count > 0)
                {
                    accumulated.minBounds = glm::min(accumulated.minBounds, bins[i].minBounds);
                    accumulated.maxBounds = glm::max(accumulated.maxBounds, bins[i].maxBounds);
                    accumulated.count += bins[i].count;
                }

                prefix[i] = accumulated;
            }

            accumulated = Bin{};
            for (uint32 i = BIN_COUNT; i-- > 0;)
            {
                if (bins[i].count > 0)
                {
                    accumulated.minBounds = glm::min(accumulated.minBounds, bins[i].minBounds);
                    accumulated.maxBounds = glm::max(accumulated.maxBounds, bins[i].maxBounds);
                    accumulated.count += bins[i].count;
                }

                suffix[i] = accumulated;
            }

            // 5. 분할 후보의 SAH Cost 계산
            for (uint32 split = 0; split < BIN_COUNT - 1; ++split)
            {
                const Bin& left = prefix[split];
                const Bin& right = suffix[split + 1];
                if (left.count == 0 || right.count == 0)
                    continue;

                GPUBVHNodeBounds leftBounds{};
                leftBounds.aabbMin = vec4(left.minBounds, 0.0f);
                leftBounds.aabbMax = vec4(left.maxBounds, 0.0f);

                GPUBVHNodeBounds rightBounds{};
                rightBounds.aabbMin = vec4(right.minBounds, 0.0f);
                rightBounds.aabbMax = vec4(right.maxBounds, 0.0f);

                const float leftArea = CalculateSurfaceArea(leftBounds);
                const float rightArea = CalculateSurfaceArea(rightBounds);
                const float cost = TRAVERSAL_COST + (leftArea / parentArea) * left.count * INTERSECTION_COST + (rightArea / parentArea) * right.count * INTERSECTION_COST;
                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestAxis = axis;
                    bestSplit = split;
                    found = true;
                }
            }
        }

        // 6. 분할 비용이 Leaf 유지보다 크면 종료
        if (!found) return false;

        // 7. 가장 낮은 비용의 분할 위치 계산
        const float splitPosition = centroidMin[bestAxis] + centroidExtent[bestAxis] * (static_cast<float>(bestSplit + 1) / BIN_COUNT);

        // 8. 실제 Primitive 배열 분할
        auto middle = std::partition
        (
            indices.begin() + begin,
            indices.begin() + end,
            [&](uint32 primitiveIndex)
            {
                const GPUBVHNodeBounds& primitive = primitiveBounds[primitiveIndex];
                const float centroid = (primitive.aabbMin[bestAxis] + primitive.aabbMax[bestAxis]) * 0.5f;
                return centroid < splitPosition;
            }
        );

        splitIndex = static_cast<uint32>(std::distance(indices.begin(), middle));
        return splitIndex > begin && splitIndex < end;
    }
}
