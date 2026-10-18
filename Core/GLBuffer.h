#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>

namespace bdt
{
	BDT_CLASS_PTR(GLBuffer)

	class GLBuffer
	{
		BDT_DISABLE_COPY(GLBuffer)
		BDT_DISABLE_MOVE(GLBuffer)

	public:
		~GLBuffer();
		static GLBufferUPtr Create(usize size, const void* data = nullptr, GLbitfield storageFlag = 0);
		void SetData(usize offset, usize size, const void* data);
		void BindBase(GLenum target, uint32 bindingPoint) const;

	public:
		uint32 GetBufferID() const;
		usize GetBufferSize() const;
		bool IsValid() const;

	private:
		GLBuffer();

		uint32 m_bufferID{ 0 };
		usize m_bufferSize{ 0 };
		GLbitfield m_storageFlags{ 0 };
	};
}
