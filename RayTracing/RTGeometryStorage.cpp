#include "pch.h"
#include "RTGeometryStorage.h"
#include "Enums.h"

namespace bdt
{
	RTGeometryStorage::RTGeometryStorage() = default;
	RTGeometryStorage::~RTGeometryStorage() = default;

    RTGeometryStorageUPtr RTGeometryStorage::Create()
    {
        RTGeometryStorageUPtr storage(new RTGeometryStorage());
        if (!storage->Init()) return nullptr;
        return storage;
    }

    bool RTGeometryStorage::Init()
    {
        m_staticVertexBuffer = GLStorageBuffer::Create();
        m_skinnedVertexBuffer = GLStorageBuffer::Create();
        m_indexBuffer = GLStorageBuffer::Create();
        m_descriptorBuffer = GLStorageBuffer::Create();
        if (!m_staticVertexBuffer  ||
            !m_skinnedVertexBuffer ||
            !m_indexBuffer         ||
            !m_descriptorBuffer) return false;
        return true;
    }

    void RTGeometryStorage::Clear()
    {
        // CPU Geometry Cache
        m_vertices.clear();
        m_skinnedVertices.clear();
        m_indices.clear();
        m_descriptors.clear();

        // Mesh Cache
        m_meshes.clear();

        m_isStorageDirty = false;
    }

    void RTGeometryStorage::Shutdown()
    {
        // GPU Storage
        m_staticVertexBuffer.reset();
        m_skinnedVertexBuffer.reset();
        m_indexBuffer.reset();
        m_descriptorBuffer.reset();

        // CPU Geometry Cache
        Clear();

        // Mesh Cache
        m_meshes.clear();
    }

    bool RTGeometryStorage::RegisterMesh(const MeshPtr& mesh, bool isSkinned, uint32& idx)
    {
        if (!mesh || !mesh->IsValid())
            return false;

        // 이미 등록된 Mesh라면 기존 Index 반환
        for (usize i = 0; i < m_meshes.size(); ++i)
        {
            if (m_meshes[i].get() == mesh.get())
            {
                idx = static_cast<uint32>(i);
                return true;
            }
        }

        // 새로운 Mesh 등록
        if (isSkinned) return RegisterSkinnedMesh(mesh, idx);
        return RegisterStaticMesh(mesh, idx);
    }

    bool RTGeometryStorage::RegisterStaticMesh(const MeshPtr& mesh, uint32& idx)
    {
        const StaticMesh* staticMesh = static_cast<const StaticMesh*>(mesh.get());
        const List<GLStaticVertex>& vertices = staticMesh->GetVertices();
        const List<uint32>& indices = staticMesh->GetIndices();
        if (vertices.empty() || indices.empty()) return false;
        if (indices.size() % 3 != 0) return false;

        // GPU 인덱스 범위 검사
        if (m_meshes.size() >= MaxValue<uint32>)
            return false;

        if (vertices.size() > static_cast<uint64>(MaxValue<uint32>) - m_vertices.size())
            return false;

        if (indices.size() > static_cast<uint64>(MaxValue<uint32>) - m_indices.size())
            return false;

        // Vertex Index 유효성 검사
        for (uint32 index : indices)
        {
            if (index >= vertices.size())
                return false;
        }

        GPUMeshDescriptor descriptor{};
        descriptor.vertexOffset = static_cast<uint32>(m_vertices.size());
        descriptor.vertexCount = static_cast<uint32>(vertices.size());
        descriptor.triangleOffset = static_cast<uint32>(m_indices.size() / 3);
        descriptor.triangleCount = static_cast<uint32>(indices.size() / 3);

        // CPU Vertex Cache에 추가
        m_vertices.reserve(m_vertices.size() + vertices.size());
        for (const GLStaticVertex& vertex : vertices)
        {
            GPUStaticVertex gpuVertex{};
            gpuVertex.position = vec4(vertex.position, 1.0f);
            gpuVertex.normal = vec4(vertex.normal, 0.0f);
            gpuVertex.uv = vec4(vertex.texCoord, 0.0f, 0.0f);
            m_vertices.push_back(gpuVertex);
        }

        // CPU Index Cache에 추가
        m_indices.reserve(m_indices.size() + indices.size());
        m_indices.insert(m_indices.end(), indices.begin(),indices.end());

        // Mesh Descriptor 등록
        idx = static_cast<uint32>(m_descriptors.size());
        m_descriptors.push_back(descriptor);
        m_meshes.push_back(mesh);

        m_isStorageDirty = true;
        return true;
    }

    bool RTGeometryStorage::RegisterSkinnedMesh(const MeshPtr& mesh, uint32& idx)
    {
        const SkinnedMesh* skinnedMesh = static_cast<const SkinnedMesh*>(mesh.get());
        const List<GLSkinnedVertex>& vertices = skinnedMesh->GetVertices();
        const List<uint32>& indices = skinnedMesh->GetIndices();
        if (vertices.empty() || indices.empty()) return false;
        if (indices.size() % 3 != 0) return false;

        // GPU 인덱스 범위 검사
        if (m_meshes.size() >= MaxValue<uint32>)
            return false;

        if (vertices.size() > static_cast<uint64>(MaxValue<uint32>) - m_skinnedVertices.size())
            return false;

        if (indices.size() > static_cast<uint64>(MaxValue<uint32>) - m_indices.size())
            return false;

        for (uint32 index : indices)
        {
            if (index >= vertices.size())
                return false;
        }

        GPUMeshDescriptor descriptor{};
        descriptor.vertexOffset = static_cast<uint32>(m_skinnedVertices.size());
        descriptor.vertexCount = static_cast<uint32>(vertices.size());
        descriptor.triangleOffset = static_cast<uint32>(m_indices.size() / 3);
        descriptor.triangleCount = static_cast<uint32>(indices.size() / 3);

        // CPU Skinned Vertex Cache에 추가
        m_skinnedVertices.reserve(m_skinnedVertices.size() + vertices.size());
        for (const GLSkinnedVertex& vertex : vertices)
        {
            GPUSkinnedVertex gpuVertex{};
            gpuVertex.position = vec4(vertex.position, 1.0f);
            gpuVertex.normal = vec4(vertex.normal, 0.0f);
            gpuVertex.tangent = vec4(vertex.tangent, 0.0f);
            gpuVertex.uv = vec4(vertex.texCoord, 0.0f, 0.0f);

            for (uint32 i = 0; i < 4; ++i)
            {
                const int32 boneID = vertex.boneIDs[i];
                const float weight = vertex.weights[i];

                if (boneID >= 0 && weight > 0.0f)
                {
                    gpuVertex.boneIndices[i] = static_cast<uint32>(boneID);
                    gpuVertex.boneWeights[i] = weight;
                }
                else
                {
                    gpuVertex.boneIndices[i] = 0;
                    gpuVertex.boneWeights[i] = 0.0f;
                }
            }

            m_skinnedVertices.push_back(gpuVertex);
        }

        // CPU Index Cache에 추가
        m_indices.reserve(m_indices.size() + indices.size());
        m_indices.insert(m_indices.end(), indices.begin(), indices.end());

        idx = static_cast<uint32>(m_descriptors.size());
        m_descriptors.push_back(descriptor);
        m_meshes.push_back(mesh);

        m_isStorageDirty = true;
        return true;
    }

    bool RTGeometryStorage::Upload()
    {
        if (!m_isStorageDirty) return true;
        if (!UploadStaticVertexBuffer()) return false;
        if (!UploadSkinnedVertexBuffer()) return false;
        if (!UploadIndexBuffer()) return false;
        if (!UploadDescriptorBuffer()) return false;
        m_isStorageDirty = false;
        return true;
    }

    bool RTGeometryStorage::UploadStaticVertexBuffer()
    {
        if (m_vertices.empty()) return true;
        const uint64 vertexBytes = static_cast<uint64>(m_vertices.size()) * sizeof(GPUStaticVertex);
        if (vertexBytes > MaxValue<uint32>) return false;
        return m_staticVertexBuffer->Update(m_vertices.data(),static_cast<uint32>(vertexBytes));
    }

    bool RTGeometryStorage::UploadSkinnedVertexBuffer()
    {
        if (m_skinnedVertices.empty()) return true;
        const uint64 skinnedVertexBytes = static_cast<uint64>(m_skinnedVertices.size()) * sizeof(GPUSkinnedVertex);
        if (skinnedVertexBytes > MaxValue<uint32>) return false;
        return m_skinnedVertexBuffer->Update(m_skinnedVertices.data(),static_cast<uint32>(skinnedVertexBytes));
    }

    bool RTGeometryStorage::UploadIndexBuffer()
    {
        if (m_indices.empty()) return true;
        const uint64 indexBytes = static_cast<uint64>(m_indices.size()) * sizeof(uint32);
        if (indexBytes > MaxValue<uint32>) return false;
        return m_indexBuffer->Update(m_indices.data(),static_cast<uint32>(indexBytes));
    }

    bool RTGeometryStorage::UploadDescriptorBuffer()
    {
        if (m_descriptors.empty()) return true;
        const uint64 descriptorBytes = static_cast<uint64>(m_descriptors.size()) * sizeof(GPUMeshDescriptor);
        if (descriptorBytes > MaxValue<uint32>) return false;
        return m_descriptorBuffer->Update(m_descriptors.data(),static_cast<uint32>(descriptorBytes));
    }

    void RTGeometryStorage::BindStaticVertexBuffer() const
    {
        m_staticVertexBuffer->Bind(static_cast<uint32>(ERTStorageBinding::StaticVertex));
    }

    void RTGeometryStorage::BindSkinnedVertexBuffer() const
    {
        m_skinnedVertexBuffer->Bind(static_cast<uint32>(ERTStorageBinding::SkinnedVertex));
    }

    void RTGeometryStorage::BindIndexBuffer() const
    {
        m_indexBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Triangle));
    }

    void RTGeometryStorage::BindDescriptorBuffer() const
    {
        m_descriptorBuffer->Bind(static_cast<uint32>(ERTStorageBinding::Mesh));
    }

    GLStorageBuffer* RTGeometryStorage::GetVertexBuffer() const
    {
        return m_staticVertexBuffer.get();
    }

    GLStorageBuffer* RTGeometryStorage::GetSkinnedVertexBuffer() const
    {
        return m_skinnedVertexBuffer.get();
    }

    GLStorageBuffer* RTGeometryStorage::GetIndexBuffer() const
    {
        return m_indexBuffer.get();
    }

    GLStorageBuffer* RTGeometryStorage::GetDescriptorBuffer() const
    {
        return m_descriptorBuffer.get();
    }

    uint32 RTGeometryStorage::GetGeometryCount() const
    {
        return static_cast<uint32>(m_descriptors.size());
    }
}