#include "pch.h"
#include "RTInstanceStorage.h"
#include "Enums.h"

namespace bdt
{
    RTInstanceStorage::RTInstanceStorage() = default;
    RTInstanceStorage::~RTInstanceStorage() = default;

    RTInstanceStorageUPtr RTInstanceStorage::Create()
    {
        RTInstanceStorageUPtr storage(new RTInstanceStorage());
        if (!storage->Init()) return nullptr;
        return storage;
    }

    bool RTInstanceStorage::Init()
    {
        m_instanceBuffer = GLStorageBuffer::Create();
        m_boneBuffer = GLStorageBuffer::Create();
        m_skinnedOutputBuffer = GLStorageBuffer::Create();
        if (!m_instanceBuffer || !m_boneBuffer || !m_skinnedOutputBuffer) return false;
        return true;
    }

    void RTInstanceStorage::Clear()
    {
        // CPU Instance Data
        m_instances.clear();
        m_boneMatrices.clear();
        m_skinnedVertexCount = 0;
        m_maxSkinnedVertexCount = 0;

        // GPU Buffer는 유지하고 재사용
        m_isStorageDirty = true;
    }

    void RTInstanceStorage::Shutdown()
    {
        // GPU Storage
        m_instanceBuffer.reset();
        m_boneBuffer.reset();

        // CPU Instance Data
        m_instances.clear();
        m_boneMatrices.clear();

        m_isStorageDirty = false;
    }

    bool RTInstanceStorage::AddInstance(const GPUInstance& instance, uint32& idx)
    {
        if (m_instances.size() >= MaxValue<uint32>) return false;
        idx = static_cast<uint32>(m_instances.size());
        m_instances.push_back(instance);
        m_isStorageDirty = true;
        return true;
    }

    bool RTInstanceStorage::AppendBoneMatrices(const List<mat4>& matrices, uint32& offset)
    {
        if (matrices.size() > static_cast<uint64>(MaxValue<uint32>) - m_boneMatrices.size())
            return false;

        offset = static_cast<uint32>(m_boneMatrices.size());
        if (matrices.empty()) return true;

        m_boneMatrices.insert(m_boneMatrices.end(), matrices.begin(), matrices.end());
        m_isStorageDirty = true;
        return true;
    }

    bool RTInstanceStorage::AllocateSkinnedVertices(uint32 vertexCount, uint32& vertexOffset)
    {
        // GPU Buffer 크기 및 인덱스 범위 검사
        const uint64 totalCount = static_cast<uint64>(m_skinnedVertexCount) + vertexCount;
        const uint64 totalBytes = totalCount * sizeof(GPUStaticVertex);

        if (totalCount > MaxValue<uint32> || totalBytes > MaxValue<uint32>)
            return false;

        // 현재 Instance의 출력 정점 시작 위치
        vertexOffset = m_skinnedVertexCount;

        // 다음 할당을 위한 정점 개수 갱신
        m_skinnedVertexCount = static_cast<uint32>(totalCount);
        m_maxSkinnedVertexCount = glm::max(m_maxSkinnedVertexCount, vertexCount);
        if (vertexCount > 0) m_isStorageDirty = true;

        return true;
    }

    bool RTInstanceStorage::Upload()
    {
        if (!m_isStorageDirty) return true;
        if (!UploadInstanceBuffer()) return false;
        if (!UploadBoneBuffer()) return false;
        if (!ResizeSkinnedOutputBuffer()) return false;
        m_isStorageDirty = false;
        return true;
    }

    bool RTInstanceStorage::UploadInstanceBuffer()
    {
        if (m_instances.empty()) return true;
        const uint64 instanceBytes = static_cast<uint64>(m_instances.size()) * sizeof(GPUInstance);
        if (instanceBytes > MaxValue<uint32>) return false;
        return m_instanceBuffer->Update(m_instances.data(),static_cast<uint32>(instanceBytes));
    }

    bool RTInstanceStorage::UploadBoneBuffer()
    {
        if (m_boneMatrices.empty()) return true;
        const uint64 boneBytes = static_cast<uint64>(m_boneMatrices.size()) * sizeof(mat4);
        if (boneBytes > MaxValue<uint32>) return false;
        return m_boneBuffer->Update(m_boneMatrices.data(), static_cast<uint32>(boneBytes));
    }

    bool RTInstanceStorage::ResizeSkinnedOutputBuffer()
    {
        if (m_skinnedVertexCount == 0) return true;
        const uint64 bytes = static_cast<uint64>(m_skinnedVertexCount) * sizeof(GPUStaticVertex);
        if (bytes > MaxValue<uint32>) return false;
        return m_skinnedOutputBuffer->Resize(static_cast<uint32>(bytes));
    }

    void RTInstanceStorage::BindInstanceBuffer() const
    {
        m_instanceBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Instance));
    }

    void RTInstanceStorage::BindBoneBuffer() const
    {
        m_boneBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Bone));
    }

    void RTInstanceStorage::BindSkinnedOutputBuffer() const
    {
        m_skinnedOutputBuffer->Bind(static_cast<uint32>(ERTStorageBinding::SkinnedOutput));
    }

    uint32 RTInstanceStorage::GetInstanceCount() const
    {
        return static_cast<uint32>(m_instances.size());
    }

    uint32 RTInstanceStorage::GetBoneMatrixCount() const
    {
        return static_cast<uint32>(m_boneMatrices.size());
    }

    uint32 RTInstanceStorage::GetSkinnedVertexCount() const
    {
        return m_skinnedVertexCount;
    }

    uint32 RTInstanceStorage::GetMaxSkinnedVertexCount() const
    {
        return m_maxSkinnedVertexCount;
    }
}
