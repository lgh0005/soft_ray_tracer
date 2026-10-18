#include "pch.h"
#include "GLTextureCube.h"

namespace bdt
{
	GLTextureCube::GLTextureCube() = default;

	GLTextureCube::~GLTextureCube()
	{
		if (m_textureID == 0) return;
		glDeleteTextures(1, &m_textureID);
		m_textureID = 0;
		m_target = 0;
	}

	GLTextureCubeUPtr GLTextureCube::Create(ktxTexture* texture)
	{
		GLTextureCubeUPtr glTexture(new GLTextureCube());
		if (!glTexture->Init(texture)) return nullptr;
		return glTexture;
	}

	bool GLTextureCube::Init(ktxTexture* texture)
	{
		if (!texture) return false;

		GLenum glError = GL_NO_ERROR;
		const KTX_error_code result = ktxTexture_GLUpload
		(
			texture,
			&m_textureID,
			&m_target,
			&glError
		);

		if (result != KTX_SUCCESS || glError != GL_NO_ERROR) return false;
		if (!IsValid()) return false;

		return true;
	}

	void GLTextureCube::Bind(uint32 textureUnit) const
	{
		if (!IsValid()) return;
		glBindTextureUnit(textureUnit, m_textureID);
	}

	void GLTextureCube::Unbind(uint32 textureUnit)
	{
		glBindTextureUnit(textureUnit, 0);
	}

	uint32 GLTextureCube::GetTextureID() const { return m_textureID; }
	GLenum GLTextureCube::GetTarget() const { return m_target; }
	bool GLTextureCube::IsValid() const { return m_textureID != 0 && m_target == GL_TEXTURE_CUBE_MAP; }
}
