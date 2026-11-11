#include "pch.h"
#include "RTBVHGBuffer.h"

namespace bdt
{
	RTBVHGBuffer::RTBVHGBuffer() = default;
	RTBVHGBuffer::~RTBVHGBuffer() = default;

	RTBVHGBufferUPtr RTBVHGBuffer::Create(uint32 width, uint32 height)
	{
		RTBVHGBufferUPtr result(new RTBVHGBuffer());
		if (!result->Init(width, height)) return nullptr;
		return result;
	}

	bool RTBVHGBuffer::Init(uint32 width, uint32 height)
	{
		return false;
	}

	bool RTBVHGBuffer::Resize(uint32 width, uint32 height)
	{
		return false;
	}

	void RTBVHGBuffer::Bind() const
	{
	}

	void RTBVHGBuffer::Unbind() const
	{
	}

	void RTBVHGBuffer::Clear() const
	{
	}

	uint32 RTBVHGBuffer::GetWidth() const
	{
		return m_width;
	}

	uint32 RTBVHGBuffer::GetHeight() const
	{
		return m_height;
	}
}
