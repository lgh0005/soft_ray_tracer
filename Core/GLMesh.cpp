#include "pch.h"
#include "GLMesh.h"
#include "GLBuffer.h"

namespace bdt
{
    GLMesh::GLMesh() = default;
    GLMesh::~GLMesh() = default;

    bool GLMesh::Init
    (
        const void* vertexData, 
        usize vertexBytes, 
        uint32 vertexCount,
        const List<uint32>& indices, 
        GLenum primitive
    )
    {
        if (m_vertexBuffer) return false;
        if (indices.empty()) return false;

        // Vertex Buffer 생성
        GLBufferUPtr vertexBuffer = GLBuffer::Create(vertexBytes, vertexData);
        if (!vertexBuffer) return false;

        // Index Buffer 생성
        GLBufferUPtr indexBuffer = GLBuffer::Create(indices.size() * sizeof(uint32), indices.data());
        if (!indexBuffer) return false;

        m_vertexBuffer = std::move(vertexBuffer);
        m_indexBuffer = std::move(indexBuffer);

        m_vertexCount = vertexCount;
        m_indexCount = static_cast<uint32>(indices.size());
        m_primitive = primitive;

        return true;
    }

    uint32 GLMesh::GetVertexCount() const
    {
        return m_vertexCount;
    }

    uint32 GLMesh::GetIndexCount() const
    {
        return m_indexCount;
    }

    GLenum GLMesh::GetPrimitive() const
    {
        return m_primitive;
    }

    const GLBuffer* GLMesh::GetVertexBuffer() const
    {
        return m_vertexBuffer.get();
    }

    const GLBuffer* GLMesh::GetIndexBuffer() const
    {
        return m_indexBuffer.get();
    }

    bool GLMesh::IsValid() const
    {
        return m_vertexBuffer && m_vertexBuffer->IsValid() &&
               m_indexBuffer && m_indexBuffer->IsValid();
    }
}