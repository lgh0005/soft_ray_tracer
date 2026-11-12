#include "pch.h"
#include "RTBVHBuilder.h"
#include "InstanceManager.h"
#include <Resources/StaticMesh.h>
#include <Resources/SkinnedMesh.h>

namespace bdt
{
    RTBVHBuilder::RTBVHBuilder() = default;

    RTBVHBuilder::~RTBVHBuilder()
    {
        Shutdown();
    }

    RTBVHBuilderUPtr RTBVHBuilder::Create(EBVHBuildMode buildMode)
    {
        RTBVHBuilderUPtr builder(new RTBVHBuilder());
        if (!builder->Init(buildMode)) return nullptr;
        return builder;
    }

    bool RTBVHBuilder::Init(EBVHBuildMode buildMode)
    {
        m_storage = RTBVHStorage::Create(buildMode);
        return m_storage != nullptr;
    }

    bool RTBVHBuilder::Prepare()
    {
        if (!m_storage) return false;

        // 1. BLAS Rebuild
        if (m_blasDirty)
        {
            if (!PrepareMeshInputs()) return false;
            if (!m_storage->RebuildBLAS(m_meshInputs))
                return false;
        }

        // 2. TLAS Rebuild
        if (m_tlasDirty)
        {
            if (!PrepareInstanceBounds()) return false;
            if (!m_storage->RebuildTLAS(m_instanceBounds))
                return false;
        }

        // 3. TLAS Refit
        else if (m_tlasRefitDirty)
        {
            if (!PrepareInstanceBounds()) return false;
            if (!m_storage->RefitTLAS(m_instanceBounds))
                return false;
        }

        // 4. 변경된 GPU 데이터 업로드
        if (!m_storage->Upload())
            return false;

        // 5. 업데이트 성공 시 Dirty 해제
        m_blasDirty = false;
        m_tlasDirty = false;
        m_tlasRefitDirty = false;

        return true;
    }

    bool RTBVHBuilder::PrepareMeshInputs()
    {
        const auto& instanceManager = InstanceManager::Instance();
        const uint32 meshCount = instanceManager.GetGeometryCount();

        // 기존 Capacity 재사용
        m_vertices.resize(meshCount);
        m_indices.resize(meshCount);
        m_meshLocalBounds.resize(meshCount);
        m_meshInputs.clear();
        m_meshInputs.reserve(meshCount);

        for (uint32 meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
            const MeshPtr& mesh = instanceManager.GetMesh(meshIndex);
            const GPUMeshDescriptor& descriptor = instanceManager.GetMeshDescriptor(meshIndex);
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

            if (vertices.empty() || indices.empty() || indices.size() % 3 != 0) return false;

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

    bool RTBVHBuilder::PrepareInstanceBounds()
    {
        const List<GPUInstance>& instances = InstanceManager::Instance().GetInstances();
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

    GPUBVHNodeBounds RTBVHBuilder::TransformBounds
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

    RTBVHStorage* RTBVHBuilder::GetStorage() const
    {
        return m_storage.get();
    }

    void RTBVHBuilder::Shutdown()
    {
        m_storage.reset();
        m_vertices.clear();
        m_indices.clear();
        m_meshLocalBounds.clear();
        m_meshInputs.clear();
        m_instanceBounds.clear();
    }

    void RTBVHBuilder::MarkBLASDirty()
    {
        m_blasDirty = true;
        m_tlasDirty = true;
        m_tlasRefitDirty = false;
    }

    void RTBVHBuilder::MarkTLASDirty()
    {
        m_tlasDirty = true;
    }

    void RTBVHBuilder::MarkTLASRefitDirty()
    {
        if (!m_tlasDirty) m_tlasRefitDirty = true;
    }
}
