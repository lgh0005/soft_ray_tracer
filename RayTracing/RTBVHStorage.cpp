#include "pch.h"
#include "RTBVHStorage.h"
#include "Enums.h"

namespace bdt
{
    RTBVHStorage::RTBVHStorage() = default;

    RTBVHStorage::~RTBVHStorage()
    {
        Shutdown();
    }

    RTBVHStorageUPtr RTBVHStorage::Create(EBVHBuildMode buildMode)
    {
        RTBVHStorageUPtr storage(new RTBVHStorage());
        if (!storage->Init(buildMode)) return nullptr;
        return storage;
    }

    bool RTBVHStorage::Init(EBVHBuildMode buildMode)
    {
        m_buildMode = buildMode;
        m_topologyBuffer = GLStorageBuffer::Create();
        m_boundsBuffer = GLStorageBuffer::Create();
        m_primitiveBuffer = GLStorageBuffer::Create();
        m_descriptorBuffer = GLStorageBuffer::Create();
        return m_topologyBuffer && m_boundsBuffer && m_primitiveBuffer && m_descriptorBuffer;
    }

    /*==========================================//
    //               BVH Updates                //
    //==========================================*/
    bool RTBVHStorage::RebuildBLAS(const List<RTBVHMeshInput>& meshes, uint32 maxLeafPrimitives)
    {
        if (maxLeafPrimitives == 0) return false;
        if (meshes.size() >= MaxValue<uint32> / sizeof(GPUBVHDescriptor))
            return false;

        // Mesh Index 순서 검증
        List<const RTBVHMeshInput*> orderedMeshes(meshes.size(), nullptr);
        for (const RTBVHMeshInput& mesh : meshes)
        {
            if (!mesh.vertices || !mesh.indices) return false;
            if (mesh.meshIndex >= meshes.size() || orderedMeshes[mesh.meshIndex]) return false;
            orderedMeshes[mesh.meshIndex] = &mesh;
        }

        // 모든 Mesh Index가 채워졌는지 확인
        for (const RTBVHMeshInput* mesh : orderedMeshes) { if (!mesh) return false; }

        // 기존 BLAS 초기화
        if (!ClearBLAS()) return false;

        // BLAS 재구축
        for (const RTBVHMeshInput* mesh : orderedMeshes)
        {
            BLASUPtr blas = std::make_unique<BLAS>(mesh->meshIndex, m_buildMode);
            if (!blas->Build(*mesh->vertices, *mesh->indices, maxLeafPrimitives)) return false;
            if (!AppendBVH(*blas, mesh->meshIndex)) return false;
            m_blas.emplace_back(std::move(blas));
        }

        m_isStorageDirty = true;
        return true;
    }

    bool RTBVHStorage::RebuildTLAS(const List<GPUBVHNodeBounds>& instanceBounds, uint32 maxLeafPrimitives)
    {
        if (maxLeafPrimitives == 0) return false;
        if (instanceBounds.size() > MaxValue<uint32>) return false;

        // 1. 새로운 TLAS 구축
        TLASUPtr newTLAS = std::make_unique<TLAS>(m_buildMode);
        if (!newTLAS->Build(instanceBounds, maxLeafPrimitives))
            return false;

        // 2. 기존 TLAS 제거
        if (!ClearTLAS()) return false;

        // 3. 새로운 TLAS Descriptor Index
        const uint32 descriptorIndex = static_cast<uint32>(m_descriptors.size());

        // 4. 새로운 TLAS 패킹
        if (!AppendBVH(*newTLAS, 0xffffffffu)) return false;

        // 5. 새로운 TLAS 등록
        m_tlas = std::move(newTLAS);
        m_tlasDescIdx = descriptorIndex;

        m_isStorageDirty = true;
        m_isBoundsDirty = false;

        return true;
    }

    bool RTBVHStorage::RefitTLAS(const List<GPUBVHNodeBounds>& instanceBounds)
    {
        if (!m_tlas) return false;
        if (m_tlasDescIdx >= m_descriptors.size()) return false;
        const GPUBVHDescriptor& descriptor = m_descriptors[m_tlasDescIdx];
        const uint32 nodeOffset = descriptor.ranges.x;
        const uint32 nodeCount = descriptor.ranges.y;

        // 1. 기존 TLAS Topology를 유지하면서 Refit
        if (!m_tlas->Refit(instanceBounds))
            return false;

        // 2. 갱신된 TLAS Bounds 가져오기
        const List<GPUBVHNodeBounds>& bounds = m_tlas->GetBounds();
        if (bounds.size() != nodeCount) 
            return false;

        if (static_cast<uint64>(nodeOffset) + nodeCount > m_bounds.size())
            return false;

        // 3. 패킹된 GPU Bounds 배열에 반영
        for (uint32 i = 0; i < nodeCount; ++i)
           m_bounds[nodeOffset + i] = bounds[i];

        // 4. Bounds만 갱신하도록 표시
        m_isBoundsDirty = true;

        return true;
    }

    /*==========================================//
    //               BVH Packing                //
    //==========================================*/
    bool RTBVHStorage::AppendBVH(const BVH& bvh, uint32 meshIndex)
    {
        const auto& topology = bvh.GetTopology();
        const auto& bounds = bvh.GetBounds();
        const auto& primitives = bvh.GetPrimitiveIndices();
        if (topology.size() != bounds.size()) return false;
        const uint64 maxTopologyCount = MaxValue<uint32> / sizeof(GPUBVHNodeTopology);
        if (m_topology.size() > maxTopologyCount || topology.size() > maxTopologyCount - m_topology.size()) return false;

        const uint64 maxBoundsCount = MaxValue<uint32> / sizeof(GPUBVHNodeBounds);
        if (m_bounds.size() > maxBoundsCount || bounds.size() > maxBoundsCount - m_bounds.size()) return false;

        const uint64 maxPrimitiveCount = MaxValue<uint32> / sizeof(uint32);
        if (m_primitiveIndices.size() > maxPrimitiveCount || primitives.size() > maxPrimitiveCount - m_primitiveIndices.size()) return false;

        const uint64 maxDescriptorCount = MaxValue<uint32> / sizeof(GPUBVHDescriptor);
        if (m_descriptors.size() >= maxDescriptorCount) return false;

        const uint32 nodeOffset = static_cast<uint32>(m_topology.size());
        const uint32 primitiveOffset = static_cast<uint32>(m_primitiveIndices.size());
        const uint32 nodeCount = bvh.GetNodeCount();
        const uint32 primitiveCount = bvh.GetPrimitiveCount();
        if (bvh.IsEmpty())
        {
            if (nodeCount != 0 || primitiveCount != 0) 
                return false;
        }
        else
        {
            if (bvh.GetRootIndex() >= nodeCount) 
                return false;
        }

        for (const GPUBVHNodeTopology& node : topology)
        {
            if (node.treeTopology.w != 0u)
            {
                if (node.primitiveRange.x > primitiveCount) return false;
                if (node.primitiveRange.y > primitiveCount - node.primitiveRange.x) return false;
            }
        }
        for (uint32 primitive : primitives)
        {
            if (primitive >= primitiveCount) 
                return false;
        }

        GPUBVHDescriptor descriptor{};
        descriptor.ranges = uvec4(nodeOffset, nodeCount, primitiveOffset, primitiveCount);
        uint32 rootIndex = 0xffffffffu;
        if (!bvh.IsEmpty()) rootIndex = nodeOffset + bvh.GetRootIndex();
        descriptor.metadata = uvec4(rootIndex, meshIndex, 0u, 0u);
        m_descriptors.emplace_back(descriptor);

        for (const GPUBVHNodeTopology& node : topology)
        {
            GPUBVHNodeTopology gpuNode = node;
            if (gpuNode.treeTopology.x != 0xffffffffu) gpuNode.treeTopology.x += nodeOffset;
            if (gpuNode.treeTopology.y != 0xffffffffu) gpuNode.treeTopology.y += nodeOffset;
            if (gpuNode.treeTopology.z != 0xffffffffu) gpuNode.treeTopology.z += nodeOffset;
            if (gpuNode.primitiveRange.w != 0xffffffffu) gpuNode.primitiveRange.w += nodeOffset;
            if (gpuNode.treeTopology.w != 0u) gpuNode.primitiveRange.x += primitiveOffset;
            m_topology.emplace_back(gpuNode);
        }

        m_bounds.insert(m_bounds.end(), bounds.begin(), bounds.end());
        m_primitiveIndices.insert(m_primitiveIndices.end(), primitives.begin(), primitives.end());
        return true;
    }

    /*==========================================//
    //            GPU Upload Helpers            //
    //==========================================*/
    bool RTBVHStorage::UploadBuffers()
    {
        const uint64 topologySize = static_cast<uint64>(m_topology.size()) * sizeof(GPUBVHNodeTopology);
        if (topologySize > MaxValue<uint32>) return false;
        if (!m_topologyBuffer->Update(m_topology.data(), static_cast<uint32>(topologySize))) return false;

        const uint64 boundsSize = static_cast<uint64>(m_bounds.size()) * sizeof(GPUBVHNodeBounds);
        if (boundsSize > MaxValue<uint32>) return false;
        if (!m_boundsBuffer->Update(m_bounds.data(), static_cast<uint32>(boundsSize))) return false;

        const uint64 primitiveSize = static_cast<uint64>(m_primitiveIndices.size()) * sizeof(uint32);
        if (primitiveSize > MaxValue<uint32>) return false;
        if (!m_primitiveBuffer->Update(m_primitiveIndices.data(), static_cast<uint32>(primitiveSize))) return false;

        const uint64 descriptorSize = static_cast<uint64>(m_descriptors.size()) * sizeof(GPUBVHDescriptor);
        if (descriptorSize > MaxValue<uint32>) return false;
        return m_descriptorBuffer->Update(m_descriptors.data(), static_cast<uint32>(descriptorSize));
    }

    bool RTBVHStorage::UploadBounds()
    {
        if (!m_boundsBuffer) return false;

        const uint64 bytes = static_cast<uint64>(m_bounds.size()) * sizeof(GPUBVHNodeBounds);
        if (bytes > MaxValue<uint32>)
            return false;

        return m_boundsBuffer->Update(m_bounds.data(), static_cast<uint32>(bytes));
    }

    /*==========================================//
    //           GPU Upload / Binding           //
    //==========================================*/
    bool RTBVHStorage::Upload()
    {
        // 1. BVH 구조가 변경된 경우 전체 업로드
        if (m_isStorageDirty)
        {
            if (!UploadBuffers()) return false;
            m_isStorageDirty = false;
            m_isBoundsDirty = false;
            return true;
        }

        // 2. BVH Bounds만 변경된 경우
        if (m_isBoundsDirty)
        {
            if (!UploadBounds()) return false;
            m_isBoundsDirty = false;
        }

        return true;
    }

    void RTBVHStorage::Bind() const
    {
        if (!m_topologyBuffer || !m_boundsBuffer || !m_primitiveBuffer || !m_descriptorBuffer) return;
        m_topologyBuffer->Bind(static_cast<uint32>(ERTStorageBinding::BVHTopology));
        m_boundsBuffer->Bind(static_cast<uint32>(ERTStorageBinding::BVHBounds));
        m_primitiveBuffer->Bind(static_cast<uint32>(ERTStorageBinding::BVHPrimitive));
        m_descriptorBuffer->Bind(static_cast<uint32>(ERTStorageBinding::BVHDescriptor));
    }

    /*==========================================//
    //             Clear / Shutdown             //
    //==========================================*/
    void RTBVHStorage::Clear()
    {
        m_blas.clear();
        m_tlas.reset();

        m_topology.clear();
        m_bounds.clear();
        m_primitiveIndices.clear();
        m_descriptors.clear();

        m_tlasDescIdx = 0xffffffffu;

        m_isStorageDirty = true;
        m_isBoundsDirty = false;
    }

    bool RTBVHStorage::ClearBLAS()
	{
        // BLAS를 제거하기 전에 TLAS부터 제거
        if (!ClearTLAS()) return false;

        m_blas.clear();
        m_topology.clear();
        m_bounds.clear();
        m_primitiveIndices.clear();
        m_descriptors.clear();

        m_isStorageDirty = true;
        m_isBoundsDirty = false;

        return true;
	}

    bool RTBVHStorage::ClearTLAS()
    {
        if (!m_tlas) return m_tlasDescIdx == 0xffffffffu;
        if (m_tlasDescIdx >= m_descriptors.size()) return false;
        if (m_tlasDescIdx != m_descriptors.size() - 1) return false;

        const GPUBVHDescriptor& descriptor = m_descriptors[m_tlasDescIdx];
        const uint32 nodeOffset = descriptor.ranges.x;
        const uint32 nodeCount = descriptor.ranges.y;
        const uint32 primitiveOffset = descriptor.ranges.z;
        const uint32 primitiveCount = descriptor.ranges.w;

        // TLAS는 항상 각 배열의 마지막 영역
        if (static_cast<uint64>(nodeOffset) + nodeCount != m_topology.size())
            return false;

        if (static_cast<uint64>(nodeOffset) + nodeCount != m_bounds.size())
            return false;

        if (static_cast<uint64>(primitiveOffset) + primitiveCount != m_primitiveIndices.size())
            return false;

        // TLAS 영역 제거
        m_topology.resize(nodeOffset);
        m_bounds.resize(nodeOffset);
        m_primitiveIndices.resize(primitiveOffset);
        m_descriptors.pop_back();

        m_tlas.reset();
        m_tlasDescIdx = 0xffffffffu;
        m_isStorageDirty = true;
        m_isBoundsDirty = false;

        return true;
    }

    void RTBVHStorage::Shutdown()
    {
        Clear();
        m_topologyBuffer.reset();
        m_boundsBuffer.reset();
        m_primitiveBuffer.reset();
        m_descriptorBuffer.reset();
        m_isStorageDirty = false;
        m_isBoundsDirty = false;
    }

    /*==========================================//
    //               Data Access                //
    //==========================================*/
    const BLAS* RTBVHStorage::GetBLAS(uint32 meshIndex) const
    {
        if (meshIndex >= m_blas.size()) return nullptr;
        return m_blas[meshIndex].get();
    }

    const TLAS* RTBVHStorage::GetTLAS() const
    {
        return m_tlas.get();
    }

	uint32 RTBVHStorage::GetTLASDescriptorIndex() const
	{
		return m_tlasDescIdx;
	}

    bool RTBVHStorage::IsStorageDirty() const
    {
        return m_isStorageDirty;
    }
}
