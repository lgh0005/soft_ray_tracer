#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>

struct ktxTexture;

namespace bdt
{
	BDT_CLASS_PTR(GLTextureCube)

	class GLTextureCube
	{
		BDT_DISABLE_COPY(GLTextureCube)
		BDT_DISABLE_MOVE(GLTextureCube)

	public:
		~GLTextureCube();
		static GLTextureCubeUPtr Create(ktxTexture* texture);

	public:
		void Bind(uint32 textureUnit) const;
		static void Unbind(uint32 textureUnit);

	public:
		uint32 GetTextureID() const;
		GLenum GetTarget() const;
		bool IsValid() const;

	private:
		GLTextureCube();
		bool Init(ktxTexture* texture);

		uint32 m_textureID{ 0 };
		GLenum m_target{ 0 };
	};
}
