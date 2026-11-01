#include "pch.h"
#include "InstanceManager.h"
#include "RTGeometryStorage.h"
#include "RTInstanceStorage.h"

namespace bdt
{
    InstanceManager::InstanceManager() = default;
    InstanceManager::~InstanceManager() = default;

    bool InstanceManager::Init()
    {
        m_geometryStorage = RTGeometryStorage::Create();
        m_instanceStorage = RTInstanceStorage::Create();
        if (!m_geometryStorage || !m_instanceStorage) return false;
        return true;
    }

    void InstanceManager::Shutdown()
    {
        if (!m_geometryStorage) return;
        m_geometryStorage->Shutdown();
        m_geometryStorage.reset();

        if (!m_instanceStorage) return;
        m_instanceStorage->Shutdown();
        m_instanceStorage.reset();
    }

    bool InstanceManager::Prepare(const RenderScene& scene)
    {
        // Instance와 Bone Matrix만 프레임마다 초기화
        ClearInstances();

        // RenderScene의 개별 RenderObject
        for (const RenderObject* object : scene.GetObjects())
        {
            if (!object) continue;
            if (!AppendRenderObject(*object)) return false;
        }

        // RenderScene의 RenderObject 그룹
        for (const List<RenderObject>* group : scene.GetObjectGroups())
        {
            if (!group) continue;
            for (const RenderObject& object : *group)
            {
                if (!AppendRenderObject(object)) return false;
            }
        }

        return Upload();
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
        instance.materialIndex = 0;

        // 5. Skinning 데이터 등록
        if (object.IsSkinned())
        {
            // 5-1. Bone Matrix 등록
            uint32 boneOffset = 0;

            if (!AppendBoneMatrices(*boneMatrices, boneOffset))
                return false;

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
        if (!AddInstance(instance, instanceIndex)) return false;

        return true;
    }

    bool InstanceManager::Upload()
    {
        if (!UploadGeometry()) return false;
        if (!UploadInstances()) return false;
        return true;
    }

    /*==================================//
    //    Geometries for ray tracing    //
    //==================================*/
    void InstanceManager::ClearGeometry()
    {
        m_geometryStorage->Clear();
    }

    bool InstanceManager::RegisterMesh(const MeshPtr& mesh, bool isSkinned, uint32& idx)
    {
        return m_geometryStorage->RegisterMesh(mesh, isSkinned, idx);
    }

    bool InstanceManager::UploadGeometry()
    {
        return m_geometryStorage->Upload();
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

    bool InstanceManager::AddInstance(const GPUInstance& instance, uint32& idx)
    {
        return m_instanceStorage->AddInstance(instance, idx);
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