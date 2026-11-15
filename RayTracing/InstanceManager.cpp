#include "pch.h"
#include "InstanceManager.h"
#include "RTGeometryStorage.h"
#include "RTInstanceStorage.h"
#include "RTMaterialStorage.h"

namespace bdt
{
    InstanceManager::InstanceManager() = default;
    InstanceManager::~InstanceManager() = default;

    bool InstanceManager::Init()
    {
        m_geometryStorage = RTGeometryStorage::Create();
        m_instanceStorage = RTInstanceStorage::Create();
        m_materialStorage = RTMaterialStorage::Create();
        if (!m_geometryStorage || !m_instanceStorage || !m_materialStorage) return false;

        return true;
    }

    void InstanceManager::Shutdown()
    {
        m_geometryStorage->Shutdown();
        m_instanceStorage->Shutdown();
        m_materialStorage->Shutdown();
    }

    bool InstanceManager::Prepare(const RenderScene& scene)
    {
        // 머티리얼 GPU 표현은 유지하고 Instance / Bone Matrix만 프레임마다 초기화
        ClearInstances();

        // RenderScene의 개별 RenderObject
        for (const RenderObject* object : scene.GetObjects())
        {
            if (!object) continue;
            if (!AppendRenderObject(*object))
                return false;
        }

        // RenderScene의 RenderObject 그룹
        for (const List<RenderObject>* group : scene.GetObjectGroups())
        {
            if (!group) continue;
            for (const RenderObject& object : *group)
            {
                if (!AppendRenderObject(object))
                    return false;
            }
        }

        return true;
    }

    bool InstanceManager::AppendRenderObject(const RenderObject& object)
    {
        // 1. Mesh 확인
        const MeshPtr& mesh = object.GetMesh();
        if (!mesh || !mesh->IsValid()) return true;

        // 2. Bone Matrix 확인
        const List<mat4>* boneMatrices = nullptr;
        if (object.IsSkinned())
        {
            boneMatrices = object.GetBoneMatrices();
            if (!boneMatrices || boneMatrices->empty()) return true;
        }

        // 3. Geometry 등록 및 Index 획득
        uint32 geometryIndex = 0;
        if (!RegisterMesh(mesh, object.IsSkinned(), geometryIndex)) return false;

        // 4. GPUInstance 생성
        GPUInstance instance{};
        instance.worldMatrix = object.GetWorldMatrix();
        instance.inverseWorldMatrix = glm::inverse(instance.worldMatrix);
        instance.meshIndex = geometryIndex;
        instance.blasBoundsOffset = MaxValue<uint32>;

        instance.materialIndex = m_materialStorage->RegisterMaterial(object.GetMaterial());
        if (instance.materialIndex == MaxValue<uint32>) return false;

        // 5. Skinning 데이터 등록
        if (object.IsSkinned())
        {
            // 5-1. Bone Matrix 등록
            if (!boneMatrices) return false;
            uint32 boneOffset = 0;
            if (!AppendBoneMatrices(*boneMatrices, boneOffset)) return false;

            instance.boneOffset = boneOffset;
            instance.boneCount = static_cast<uint32>(boneMatrices->size());

            // 5-2. SkinnedMesh의 정점 개수 확인
            const SkinnedMesh* skinnedMesh = static_cast<const SkinnedMesh*>(mesh.get());
            const List<GLSkinnedVertex>& vertices = skinnedMesh->GetVertices();

            // 5-3. Instance별 GPU Skinning 출력 영역 할당
            uint32 skinnedVertexOffset = 0;
            if (!AllocateSkinnedVertices(static_cast<uint32>(vertices.size()), skinnedVertexOffset))
                return false;

            instance.skinnedVertexOffset = skinnedVertexOffset;
        }

        // 6. GPUInstance 등록
        uint32 instanceIndex = 0;
        if (!AddInstance(instance, instanceIndex, &object)) return false;

        return true;
    }

    void InstanceManager::BindMaterialBuffer() const
    {
        m_materialStorage->Bind();
    }

    uint32 InstanceManager::GetMaterialCount() const
    {
        return m_materialStorage->GetMaterialCount();
    }

    bool InstanceManager::Upload()
    {
        if (!UploadGeometry()) return false;
        if (!UploadInstances()) return false;
        return m_materialStorage->Upload();
    }

    /*==================================//
    //    Geometries for ray tracing    //
    //==================================*/
    void InstanceManager::ClearGeometry()
    {
        m_geometryStorage->Clear();
        m_materialStorage->Clear();
    }

    bool InstanceManager::RegisterMesh(const MeshPtr& mesh, bool isSkinned, uint32& idx)
    {
        return m_geometryStorage->RegisterMesh(mesh, isSkinned, idx);
    }

    bool InstanceManager::UploadGeometry()
    {
        return m_geometryStorage->Upload();
    }

    const MeshPtr& InstanceManager::GetMesh(uint32 meshIndex) const
    {
        return m_geometryStorage->GetMesh(meshIndex);
    }

    const GPUMeshDescriptor& InstanceManager::GetMeshDescriptor(uint32 meshIndex) const
    {
        return m_geometryStorage->GetMeshDescriptor(meshIndex);
    }

    void InstanceManager::BindStaticVertexBuffer() const
    {
        m_geometryStorage->BindStaticVertexBuffer();
    }

    void InstanceManager::BindSkinnedVertexBuffer() const
    {
        m_geometryStorage->BindSkinnedVertexBuffer();
    }

    void InstanceManager::BindIndexBuffer() const
    {
        m_geometryStorage->BindIndexBuffer();
    }

    void InstanceManager::BindDescriptorBuffer() const
    {
        m_geometryStorage->BindDescriptorBuffer();
    }

    uint32 InstanceManager::GetGeometryCount() const
    {
        return m_geometryStorage->GetGeometryCount();
    }

    /*==================================//
    //     Instances for ray tracing    //
    //==================================*/
    void InstanceManager::ClearInstances()
    {
        m_instanceStorage->Clear();
    }

    bool InstanceManager::UploadInstances()
    {
        return m_instanceStorage->Upload();
    }

    const List<GPUInstance>& InstanceManager::GetInstances() const
    {
        return m_instanceStorage->GetInstances();
    }

    const List<mat4>& InstanceManager::GetBoneMatrices() const
    {
        return m_instanceStorage->GetBoneMatrices();
    }
    
    const List<const RenderObject*>& InstanceManager::GetInstanceSources() const
    {
        return m_instanceStorage->GetInstanceSources();
    }
    
    bool InstanceManager::AddInstance(const GPUInstance& instance, uint32& idx, const RenderObject* source)
    {
        return m_instanceStorage->AddInstance(instance, idx, source);
    }

    bool InstanceManager::AppendBoneMatrices(const List<mat4>& matrices, uint32& offset)
    {
        return m_instanceStorage->AppendBoneMatrices(matrices, offset);
    }

    void InstanceManager::BindInstanceBuffer() const
    {
        m_instanceStorage->BindInstanceBuffer();
    }

    void InstanceManager::BindBoneBuffer() const
    {
        m_instanceStorage->BindBoneBuffer();
    }

    uint32 InstanceManager::GetInstanceCount() const
    {
        return m_instanceStorage->GetInstanceCount();
    }

    uint32 InstanceManager::GetBoneMatrixCount() const
    {
        return m_instanceStorage->GetBoneMatrixCount();
    }

    bool InstanceManager::AllocateSkinnedVertices(uint32 vertexCount, uint32& vertexOffset)
    {
        return m_instanceStorage->AllocateSkinnedVertices(vertexCount, vertexOffset);
    }

    bool InstanceManager::UpdateBLASBoundsOffsets( const List<uint32>& offsets)
    {
        return m_instanceStorage->UpdateBLASBoundsOffsets(offsets);
    }

    void InstanceManager::BindSkinnedOutputBuffer() const
    {
        m_instanceStorage->BindSkinnedOutputBuffer();
    }

    uint32 InstanceManager::GetSkinnedVertexCount() const
    {
        return m_instanceStorage->GetSkinnedVertexCount();
    }
    uint32 InstanceManager::GetMaxSkinnedVertexCount() const
    {
        return m_instanceStorage->GetMaxSkinnedVertexCount();
    }
}