#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>

struct ktxTexture;

namespace bdt
{
	BDT_CLASS_PTR(GLTexture2D)

	class GLTexture2D
	{
		BDT_DISABLE_COPY(GLTexture2D)
		BDT_DISABLE_MOVE(GLTexture2D)

	public:
		~GLTexture2D();
		static GLTexture2DUPtr Create(ktxTexture* texture);

	public:
		void Bind(uint32 textureUnit) const;
		static void Unbind(uint32 textureUnit);

	public:
		uint32 GetTextureID() const;
		GLenum GetTarget() const;
		bool IsValid() const;

	private:
		GLTexture2D();
		bool Init(ktxTexture* texture);

		uint32 m_textureID{ 0 };
		GLenum m_target{ 0 };
	};
}
