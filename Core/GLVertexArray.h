#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>

namespace bdt
{
	BDT_CLASS_PTR(GLBuffer)
	BDT_CLASS_PTR(GLVertexArray)

	class GLVertexArray
	{
		BDT_DISABLE_COPY(GLVertexArray)
		BDT_DISABLE_MOVE(GLVertexArray)

	public:
		~GLVertexArray();
		static GLVertexArrayUPtr Create();
		void Bind() const;
		static void Unbind();

	public:
		void SetVertexBuffer
		(
			const GLBuffer& vertexBuffer,
			uint32 bindingIndex, 
			usize stride, 
			uint64 bufferOffset = 0
		) const;

		void SetIndexBuffer(const GLBuffer& indexBuffer) const;

	public:
		void SetAttrib
		(
			uint32 attribIndex,
			uint32 bindingIndex,
			int32 count,
			GLenum type,
			bool normalized,
			uint64 relativeOffset
		) const;

		void SetAttribI
		(
			uint32 attribIndex,
			uint32 bindingIndex,
			int32 count,
			GLenum type,
			uint64 relativeOffset
		) const;

		void SetAttribDivisor
		(
			uint32 bindingIndex,
			uint32 divisor
		) const;

		void DisableAttrib(uint32 attribIndex) const;

	public:
		uint32 GetVertexArrayID() const;
		bool IsValid() const;

	private:
		GLVertexArray();
		bool Init();
		uint32 m_vertexArrayID { 0 };
	};
}
