#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
	BDT_CLASS_PTR(GLFramebuffer2D)

	class GLFramebuffer2D
	{
		BDT_DISABLE_COPY(GLFramebuffer2D)
		BDT_DISABLE_MOVE(GLFramebuffer2D)

	public:
		~GLFramebuffer2D();
		static GLFramebuffer2DUPtr Create(uint32 width, uint32 height);

	public:
		void Bind() const;
		static void Unbind();

		bool AttachColor(uint32 index, uint32 textureID);
		bool AttachDepth(uint32 textureID);
		bool AttachStencil(uint32 textureID);
		bool AttachDepthStencil(uint32 textureID);

		void SetDrawBuffers(uint32 count);
		bool IsComplete() const;

	public:
		uint32 GetFramebufferID() const;
		uint32 GetWidth() const;
		uint32 GetHeight() const;
		bool IsValid() const;

	private:
		GLFramebuffer2D();
		bool Init(uint32 width, uint32 height);
		void Clear();

	private:
		uint32 m_framebufferID{ 0 };
		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
	};
}
