#include "pch.h"
#include "InstanceBuffer.h"

namespace bdt
{
    InstanceBuffer::InstanceBuffer()
    {
        m_instBufferID = CreateBuffer();
    }

    InstanceBuffer::~InstanceBuffer()
    {
        if (m_instBufferID != 0)
        {
            glDeleteBuffers(1, &m_instBufferID);
            m_instBufferID = 0;
        }
    }

    InstanceBufferUPtr InstanceBuffer::Create()
    {
        InstanceBufferUPtr buffer(new InstanceBuffer());
        if (!buffer->Init()) return nullptr;
        return buffer;
    }

    bool InstanceBuffer::Init()
    {
        m_instBufferID = CreateBuffer();
        return m_instBufferID != 0;
    }

    void InstanceBuffer::Update(const void* data, uint32 byteSize)
    {
        if (byteSize == 0 || data == nullptr)
            return;

        SetCapacity(byteSize);
        if (m_capacity < byteSize)
            return;

        glNamedBufferSubData
        (
            m_instBufferID,
            0,
            static_cast<GLsizeiptr>(byteSize),
            data
        );

        m_size = byteSize;
    }

    uint32 InstanceBuffer::CreateBuffer()
    {
        uint32 bufferID = 0;
        glCreateBuffers(1, &bufferID);
        return bufferID;
    }

    void InstanceBuffer::SetCapacity(uint32 requiredBytes)
    {
        if (requiredBytes <= m_capacity)
            return;

        // 기존 용량의 2배 또는 필요한 크기 중 큰 값 선택
        const uint64 newCapacity = glm::max<uint64>
        (
            requiredBytes, 
            static_cast<uint64>(m_capacity) * 2
        );
        if (newCapacity > MaxValue<uint32>)
            return;

        uint32 newBufferID = CreateBuffer();
        if (newBufferID == 0)
            return;

        glNamedBufferStorage
        (
            newBufferID,
            static_cast<GLsizeiptr>(newCapacity),
            nullptr,
            GL_DYNAMIC_STORAGE_BIT
        );

        // 기존 GPU 버퍼 교체
        if (m_instBufferID != 0)
            glDeleteBuffers(1, &m_instBufferID);

        m_instBufferID = newBufferID;
        m_capacity = static_cast<uint32>(newCapacity);
    }

    void InstanceBuffer::Bind(uint32 slot) const
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_instBufferID);
    }

    void InstanceBuffer::Unbind(uint32 slot)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
    }
}
