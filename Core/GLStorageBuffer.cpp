#include "pch.h"
#include "GLStorageBuffer.h"

namespace bdt
{
    GLStorageBuffer::GLStorageBuffer() = default;
    GLStorageBuffer::~GLStorageBuffer() = default;

    GLStorageBufferUPtr GLStorageBuffer::Create()
    {
        GLStorageBufferUPtr buffer(new GLStorageBuffer());
        if (!buffer->Init()) return nullptr;
        return buffer;
    }

    bool GLStorageBuffer::Init()
    {
        // 빈 상태에서도 유효한 SSBO를 바인딩할 수 있도록 최소 저장 공간을 확보한다.
        return SetCapacity(16);
    }

    bool GLStorageBuffer::Update(const void* data, uint32 byteSize)
    {
        if (byteSize == 0)
        {
            m_size = 0;
            return true;
        }

        if (data == nullptr || !SetCapacity(byteSize))
            return false;

        m_buffer->SetData(0, byteSize, data);
        m_size = byteSize;
        return true;
    }

    bool GLStorageBuffer::SetCapacity(uint32 requiredBytes)
    {
        if (requiredBytes <= m_capacity)
            return true;

        // 기존 용량의 2배 또는 필요한 크기 중 큰 값 선택 (uint32 범위 내).
        const uint64 doubledCapacity = static_cast<uint64>(m_capacity) * 2;
        const uint64 newCapacity = glm::max<uint64>
        (
            requiredBytes,
            glm::min<uint64>(doubledCapacity, MaxValue<uint32>)
        );

        GLBufferUPtr buffer = GLBuffer::Create
        (
            static_cast<usize>(newCapacity),
            nullptr,
            GL_DYNAMIC_STORAGE_BIT
        );
        if (!buffer)
            return false;

        // 할당 성공 후 교체한다. 데이터는 Update에서 전체 업로드한다.
        m_buffer = std::move(buffer);
        m_capacity = static_cast<uint32>(newCapacity);
        return true;
    }

    void GLStorageBuffer::Bind(uint32 slot) const
    {
        m_buffer->BindBase(GL_SHADER_STORAGE_BUFFER, slot);
    }

    void GLStorageBuffer::Unbind(uint32 slot)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0);
    }

    bool GLStorageBuffer::Resize(uint32 byteSize)
    {
        if (!SetCapacity(byteSize)) return false;
        m_size = byteSize;
        return true;
    }
}
