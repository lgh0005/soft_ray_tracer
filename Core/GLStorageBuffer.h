#pragma once
#include "GLBuffer.h"

namespace bdt
{
	BDT_CLASS_PTR(GLStorageBuffer)

	class GLStorageBuffer
	{
		BDT_DISABLE_COPY(GLStorageBuffer)
		BDT_DISABLE_MOVE(GLStorageBuffer)

	public:
		~GLStorageBuffer();
		static GLStorageBufferUPtr Create();

		bool Update(const void* data, uint32 byteSize);
		void Bind(uint32 slot) const;
		static void Unbind(uint32 slot);
		uint32 GetSize() const { return m_size; }
		uint32 GetCapacity() const { return m_capacity; }

	private:
		GLStorageBuffer();
		bool Init();
		bool SetCapacity(uint32 requiredBytes);

		GLBufferUPtr m_buffer{ nullptr };
		uint32 m_size{ 0 };
		uint32 m_capacity{ 0 };
	};
}
