#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>

namespace bdt
{
	BDT_CLASS_PTR(GLBuffer)
	BDT_CLASS_PTR(GLVertexArray)
	BDT_CLASS_PTR(GLMesh)

	class GLMesh
	{
		BDT_DISABLE_COPY(GLMesh)
		BDT_DISABLE_MOVE(GLMesh)

	public:
		virtual ~GLMesh();

	public:
		uint32 GetVertexCount() const;
		uint32 GetIndexCount() const;
		GLenum GetPrimitive() const;

		const GLBuffer* GetVertexBuffer() const;
		const GLBuffer* GetIndexBuffer() const;

		bool IsValid() const;

	protected:
		GLMesh();
		bool Init
		(
			const void* vertexData,
			usize vertexBytes,
			uint32 vertexCount,
			const List<uint32>& indices,
			GLenum primitive = GL_TRIANGLES
		);

	private:
		GLBufferUPtr m_vertexBuffer;
		GLBufferUPtr m_indexBuffer;

		uint32 m_vertexCount{ 0 };
		uint32 m_indexCount{ 0 };
		GLenum m_primitive{ GL_TRIANGLES };
	};
}

