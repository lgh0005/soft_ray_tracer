#include "pch.h"
#include "RTBVHUpdateStorage.h"
#include "RayTracing.h"
#include <Resources/StaticMesh.h>
#include <Resources/SkinnedMesh.h>

namespace bdt
{
    RTBVHUpdateStorage::RTBVHUpdateStorage() = default;

    RTBVHUpdateStorage::~RTBVHUpdateStorage()
    {
        Shutdown();
    }

    RTBVHUpdateStorageUPtr RTBVHUpdateStorage::Create()
    {
        RTBVHUpdateStorageUPtr storage(new RTBVHUpdateStorage());
        if (!storage->Init()) return nullptr;
        return storage;
    }

    bool RTBVHUpdateStorage::Init()
    {
        Clear();
        return true;
    }

    void RTBVHUpdateStorage::Clear()
    {
        m_meshInputs.clear();
        m_instanceBounds.clear();
        m_vertices.clear();
        m_indices.clear();
        m_meshLocalBounds.clear();
        m_previousMeshes.clear();
        m_previousDescriptors.clear();
        m_previousInstances.clear();
        m_previousSources.clear();
        m_previousBones.clear();
        m_boundsOffsets.clear();
        m_maxSkinnedNodeCount = 0;
        m_blasDirty = true;
        m_tlasDirty = true;
        m_skinnedLayoutDirty = true;
        m_gpuBLASRefitRequired = false;
        m_gpuTLASRefitRequired = false;
    }

    void RTBVHUpdateStorage::Shutdown()
    {
        Clear();
    }

    void RTBVHUpdateStorage::CompleteBuild()
    {
        SaveSnapshot();
        m_blasDirty = false;
        m_tlasDirty = false;
        m_skinnedLayoutDirty = false;
    }

    const List<RTBVHMeshInput>& RTBVHUpdateStorage::GetMeshInputs() const
    {
        return m_meshInputs;
    }

    const List<GPUBVHNodeBounds>& RTBVHUpdateStorage::GetInstanceBounds() const
    {
        return m_instanceBounds;
    }

    const List<uint32>& RTBVHUpdateStorage::GetBoundsOffsets() const
    {
        return m_boundsOffsets;
    }

    bool RTBVHUpdateStorage::IsBLASDirty() const
    {
        return m_blasDirty;
    }

    bool RTBVHUpdateStorage::IsTLASDirty() const
    {
        return m_tlasDirty;
    }

    bool RTBVHUpdateStorage::IsSkinnedLayoutDirty() const
    {
        return m_skinnedLayoutDirty;
    }

    bool RTBVHUpdateStorage::DetectChanges(const RTBVHStorage& storage)
    {
        DetectGeometryChanges();
        if (!DetectInstanceChanges()) return false;
        DetectBoneChanges();

        if (!storage.GetTLAS()) MarkTLASDirty();
        return true;
    }

    void RTBVHUpdateStorage::DetectGeometryChanges()
    {
        const uint32 meshCount = BDT_INSTANCE.GetGeometryCount();
        if (meshCount != m_previousMeshes.size()) MarkBLASDirty();
        else
        {
            for (uint32 i = 0; i < meshCount; ++i)
            {
                const auto& a = BDT_INSTANCE.GetMeshDescriptor(i);
                const auto& b = m_previousDescriptors[i];
                if (BDT_INSTANCE.GetMesh(i) != m_previousMeshes[i] ||
                    a.vertexOffset != b.vertexOffset     || 
                    a.vertexCount != b.vertexCount       ||
                    a.triangleOffset != b.triangleOffset || 
                    a.triangleCount != b.triangleCount   ||
                    a.meshType != b.meshType)
                {
                    MarkBLASDirty();
                    break;
                }
            }
        }
    }

    bool RTBVHUpdateStorage::DetectInstanceChanges()
    {
        const auto& instances = BDT_INSTANCE.GetInstances();
        const auto& sources = BDT_INSTANCE.GetInstanceSources();
        if (sources.size() != instances.size()) return false;
        if (instances.size() != m_previousInstances.size()) MarkTLASDirty();
        else
        {
            for (usize i = 0; i < instances.size(); ++i)
            {
                const auto& current = instances[i];
                const auto& previous = m_previousInstances[i];
                if (sources[i] != m_previousSources[i]        ||
                    current.meshIndex != previous.meshIndex   ||
                    current.boneOffset != previous.boneOffset ||
                    current.boneCount != previous.boneCount   ||
                    current.skinnedVertexOffset != previous.skinnedVertexOffset)
                {
                    MarkTLASDirty();
                    break;
                }

                if (current.worldMatrix != previous.worldMatrix)
                    MarkTLASRefitDirty();
            }
        }

        return true;
    }

    void RTBVHUpdateStorage::DetectBoneChanges()
    {
        const auto& bones = BDT_INSTANCE.GetBoneMatrices();
        if (bones != m_previousBones || m_skinnedLayoutDirty)
        {
            m_gpuBLASRefitRequired = true;
            m_gpuTLASRefitRequired = true;
        }
    }

    void RTBVHUpdateStorage::SaveSnapshot()
    {
        const uint32 meshCount = BDT_INSTANCE.GetGeometryCount();
        m_previousMeshes.resize(meshCount);
        m_previousDescriptors.resize(meshCount);
        for (uint32 i = 0; i < meshCount; ++i)
        {
            m_previousMeshes[i] = BDT_INSTANCE.GetMesh(i);
            m_previousDescriptors[i] = BDT_INSTANCE.GetMeshDescriptor(i);
        }
        m_previousInstances = BDT_INSTANCE.GetInstances();
        m_previousSources = BDT_INSTANCE.GetInstanceSources();
        m_previousBones = BDT_INSTANCE.GetBoneMatrices();
    }

    bool RTBVHUpdateStorage::PrepareMeshInputs()
    {
        const uint32 meshCount = BDT_INSTANCE.GetGeometryCount();

        // 기존 Capacity 재사용
        m_vertices.resize(meshCount);
        m_indices.resize(meshCount);
        m_meshLocalBounds.resize(meshCount);
        m_meshInputs.clear();
        m_meshInputs.reserve(meshCount);

        for (uint32 meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
            const MeshPtr& mesh = BDT_INSTANCE.GetMesh(meshIndex);
            const GPUMeshDescriptor& descriptor = BDT_INSTANCE.GetMeshDescriptor(meshIndex);
            if (!mesh || !mesh->IsValid()) return false;

            List<vec3>& vertices = m_vertices[meshIndex];
            List<uint32>& indices = m_indices[meshIndex];
            vertices.clear();
            indices.clear();

            switch (static_cast<ERTMeshType>(descriptor.meshType))
            {
                case ERTMeshType::Static:
                {
                    const StaticMesh* staticMesh = static_cast<const StaticMesh*>(mesh.get());
                    const List<GLStaticVertex>& sourceVertices = staticMesh->GetVertices();
                    const List<uint32>& sourceIndices = staticMesh->GetIndices();
                    vertices.reserve(sourceVertices.size());
                    for (const GLStaticVertex& vertex : sourceVertices)
                        vertices.push_back(vertex.position);
                    indices.assign(sourceIndices.begin(), sourceIndices.end());
                    break;
                }

                case ERTMeshType::Skinned:
                {
                    const SkinnedMesh* skinnedMesh = static_cast<const SkinnedMesh*>(mesh.get());
                    const List<GLSkinnedVertex>& sourceVertices = skinnedMesh->GetVertices();
                    const List<uint32>& sourceIndices = skinnedMesh->GetIndices();
                    vertices.reserve(sourceVertices.size());
                    for (const GLSkinnedVertex& vertex : sourceVertices)
                        vertices.push_back(vertex.position);
                    indices.assign(sourceIndices.begin(), sourceIndices.end());
                    break;
                }

                default:
                    return false;
            }

            // Mesh Local AABB 계산
            vec3 minBounds(MaxValue<float>);
            vec3 maxBounds(MinValue<float>);
            for (const vec3& position : vertices)
            {
                minBounds = glm::min(minBounds, position);
                maxBounds = glm::max(maxBounds, position);
            }

            GPUBVHNodeBounds& localBounds = m_meshLocalBounds[meshIndex];
            localBounds.aabbMin = vec4(minBounds, 0.0f);
            localBounds.aabbMax = vec4(maxBounds, 0.0f);
            m_meshInputs.push_back({meshIndex, &vertices, &indices});
        }
        return true;
    }

    bool RTBVHUpdateStorage::PrepareInstanceBounds()
    {
        const List<GPUInstance>& instances = BDT_INSTANCE.GetInstances();
        const uint32 meshCount = static_cast<uint32>(m_meshLocalBounds.size());
        m_instanceBounds.clear();
        m_instanceBounds.reserve(instances.size());

        for (const GPUInstance& instance : instances)
        {
            if (instance.meshIndex >= meshCount) return false;
            const GPUBVHNodeBounds& localBounds = m_meshLocalBounds[instance.meshIndex];
            m_instanceBounds.push_back(TransformBounds(localBounds, instance.worldMatrix));
        }
        return true;
    }

    GPUBVHNodeBounds RTBVHUpdateStorage::TransformBounds
    (
        const GPUBVHNodeBounds& localBounds,
        const mat4& worldMatrix
    ) const
    {
        const vec3 localMin(localBounds.aabbMin);
        const vec3 localMax(localBounds.aabbMax);
        vec3 worldMin(MaxValue<float>);
        vec3 worldMax(MinValue<float>);

        // AABB의 여덟 꼭짓점을 World Space로 변환
        for (uint32 i = 0; i < 8; ++i)
        {
            const vec3 corner
            (
                (i & 1u) ? localMax.x : localMin.x,
                (i & 2u) ? localMax.y : localMin.y,
                (i & 4u) ? localMax.z : localMin.z
            );
            const vec3 worldPosition = vec3(worldMatrix * vec4(corner, 1.0f));
            worldMin = glm::min(worldMin, worldPosition);
            worldMax = glm::max(worldMax, worldPosition);
        }

        GPUBVHNodeBounds result{};
        result.aabbMin = vec4(worldMin, 0.0f);
        result.aabbMax = vec4(worldMax, 0.0f);
        return result;
    }

    bool RTBVHUpdateStorage::PrepareSkinnedBounds(RTBVHStorage& storage)
    {
        const List<GPUInstance>& instances = BDT_INSTANCE.GetInstances();
        List<uint32> offsets(instances.size(), MaxValue<uint32>);
        uint32 maxNodeCount = 0;
        uint64 totalNodeCount = 0;

        for (usize i = 0; i < instances.size(); ++i)
        {
            const GPUInstance& instance = instances[i];

            // Mesh Index 검증
            if (instance.meshIndex >= BDT_INSTANCE.GetGeometryCount()) return false;
            const GPUMeshDescriptor& mesh = BDT_INSTANCE.GetMeshDescriptor(instance.meshIndex);

            // Static Mesh는 별도의 Bounds 영역이 필요 없음
            if (static_cast<ERTMeshType>(mesh.meshType) != ERTMeshType::Skinned)
                continue;

            // 해당 Mesh의 BLAS 조회
            const BLAS* blas =  storage.GetBLAS(instance.meshIndex);
            if (!blas) return false;

            const uint32 nodeCount = blas->GetNodeCount();
            if (nodeCount == 0) return false;
            if (instance.boneCount == 0) continue;
            maxNodeCount = std::max(maxNodeCount, nodeCount);

            // Offset / Buffer 크기 제한 확인
            const uint64 nextNodeCount = totalNodeCount + nodeCount;
            if (nextNodeCount >= MaxValue<uint32>)
                return false;

            if (nextNodeCount * sizeof(GPUBVHNodeBounds) > MaxValue<uint32>)
                return false;

            // 현재 Instance의 BLAS Bounds 시작 위치
            offsets[i] = static_cast<uint32>(totalNodeCount);
            totalNodeCount = nextNodeCount;
        }

        // 필요한 GPU Bounds 공간 확보
        if (totalNodeCount > 0)
        {
            if (!storage.ResizeSkinnedBounds(static_cast<uint32>(totalNodeCount)))
                return false;
        }

        m_boundsOffsets = std::move(offsets);
        m_maxSkinnedNodeCount = maxNodeCount;
        return true;
    }

    void RTBVHUpdateStorage::MarkBLASDirty()
    {
        m_blasDirty = true;
        m_tlasDirty = true;
        m_skinnedLayoutDirty = true;
        m_gpuBLASRefitRequired = true;
        m_gpuTLASRefitRequired = true;
    }

    void RTBVHUpdateStorage::MarkTLASDirty()
    {
        m_tlasDirty = true;
        m_skinnedLayoutDirty = true;
        m_gpuBLASRefitRequired = true;
        m_gpuTLASRefitRequired = true;
    }

    void RTBVHUpdateStorage::MarkTLASRefitDirty()
    {
        m_gpuTLASRefitRequired = true;
    }

    bool RTBVHUpdateStorage::IsBLASRefitRequired() const
    {
        return m_gpuBLASRefitRequired && m_maxSkinnedNodeCount != 0;
    }

    bool RTBVHUpdateStorage::IsTLASRefitRequired() const
    {
        return m_gpuTLASRefitRequired;
    }

    uint32 RTBVHUpdateStorage::GetMaxSkinnedNodeCount() const
    {
        return m_maxSkinnedNodeCount;
    }

    void RTBVHUpdateStorage::CompleteRefit()
    {
        m_gpuBLASRefitRequired = false;
        m_gpuTLASRefitRequired = false;
    }
}
