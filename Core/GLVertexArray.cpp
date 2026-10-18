#include "pch.h"
#include "GLVertexArray.h"
#include "GLBuffer.h"

namespace bdt
{
	GLVertexArray::~GLVertexArray()
	{
		if (m_vertexArrayID != 0)
		{
			glDeleteVertexArrays(1, &m_vertexArrayID);
			m_vertexArrayID = 0;
		}
	}

	GLVertexArrayUPtr GLVertexArray::Create()
	{
		GLVertexArrayUPtr vertexArray(new GLVertexArray());
		if (!vertexArray->Init()) return nullptr;
		return vertexArray;
	}

	bool GLVertexArray::Init()
	{
		glCreateVertexArrays(1, &m_vertexArrayID);
		if (m_vertexArrayID == 0) return false;
		return true;
	}

	void GLVertexArray::Bind() const
	{
		if (m_vertexArrayID == 0) return;
		glBindVertexArray(m_vertexArrayID);
	}

	void GLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void GLVertexArray::SetVertexBuffer
	(
		const GLBuffer& vertexBuffer,
		uint32 bindingIndex,
		usize stride,
		uint64 bufferOffset
	) const
	{
		glVertexArrayVertexBuffer(m_vertexArrayID, bindingIndex, vertexBuffer.GetBufferID(), static_cast<GLintptr>(bufferOffset), static_cast<GLsizei>(stride));
	}

	void GLVertexArray::SetIndexBuffer(const GLBuffer& indexBuffer) const
	{
		glVertexArrayElementBuffer(m_vertexArrayID, indexBuffer.GetBufferID());
	}

	void GLVertexArray::SetAttrib
	(
		uint32 attribIndex,
		uint32 bindingIndex,
		int32 count,
		GLenum type,
		bool normalized,
		uint64 relativeOffset
	) const
	{
		glEnableVertexArrayAttrib(m_vertexArrayID, attribIndex);
		glVertexArrayAttribFormat(m_vertexArrayID, attribIndex, count, type, normalized, (GLuint)relativeOffset);
		glVertexArrayAttribBinding(m_vertexArrayID, attribIndex, bindingIndex);
	}

	void GLVertexArray::SetAttribI
	(
		uint32 attribIndex,
		uint32 bindingIndex,
		int32 count,
		GLenum type,
		uint64 relativeOffset
	) const
	{
		glEnableVertexArrayAttrib(m_vertexArrayID, attribIndex);
		glVertexArrayAttribIFormat(m_vertexArrayID, attribIndex, count, type, (GLuint)relativeOffset);
		glVertexArrayAttribBinding(m_vertexArrayID, attribIndex, bindingIndex);
	}

	void GLVertexArray::SetAttribDivisor(uint32 bindingIndex, uint32 divisor) const
	{
		if (m_vertexArrayID == 0) return;
		glVertexArrayBindingDivisor(m_vertexArrayID, bindingIndex, divisor);
	}

	void GLVertexArray::DisableAttrib(uint32 attribIndex) const
	{
		if (m_vertexArrayID == 0) return;
		glDisableVertexArrayAttrib(m_vertexArrayID, attribIndex);
	}

	uint32 GLVertexArray::GetVertexArrayID() const
	{
		return m_vertexArrayID;
	}

	bool GLVertexArray::IsValid() const
	{
		return m_vertexArrayID != 0;
	}

	GLVertexArray::GLVertexArray() = default;
}
