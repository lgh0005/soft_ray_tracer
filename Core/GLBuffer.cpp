#include "pch.h"
#include "GLBuffer.h"

namespace bdt
{
	GLBuffer::GLBuffer() = default;

	GLBuffer::~GLBuffer()
	{
		if (!IsValid()) return;

		glDeleteBuffers(1, &m_bufferID);
		m_bufferID = 0;
		m_bufferSize = 0;
		m_storageFlags = 0;
	}

	GLBufferUPtr GLBuffer::Create(usize size, const void* data, GLbitfield storageFlags)
	{
		GLBufferUPtr buffer(new GLBuffer());

		glCreateBuffers(1, &buffer->m_bufferID);
		if (!buffer->IsValid()) return nullptr;

		glNamedBufferStorage(buffer->m_bufferID, static_cast<GLsizeiptr>(size), data, storageFlags);

		GLint64 allocatedSize = 0;
		glGetNamedBufferParameteri64v(buffer->m_bufferID, GL_BUFFER_SIZE, &allocatedSize);
		if (allocatedSize != static_cast<GLint64>(size)) return nullptr;

		buffer->m_bufferSize = size;
		buffer->m_storageFlags = storageFlags;

		return buffer;
	}

	void GLBuffer::SetData(usize offset, usize size, const void* data)
	{
		glNamedBufferSubData(m_bufferID, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
	}

	void GLBuffer::BindBase(GLenum target, uint32 bindingPoint) const
	{
		if (!IsValid()) return;
		glBindBufferBase(target, static_cast<GLuint>(bindingPoint), m_bufferID);
	}

	uint32 GLBuffer::GetBufferID() const
	{
		return m_bufferID;
	}

	usize GLBuffer::GetBufferSize() const
	{
		return m_bufferSize;
	}

	bool GLBuffer::IsValid() const
	{
		return m_bufferID != 0;
	}
}
