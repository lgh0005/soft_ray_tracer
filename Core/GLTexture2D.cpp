#include "pch.h"
#include "GLTexture2D.h"

namespace bdt
{
	GLTexture2D::GLTexture2D() = default;

	GLTexture2D::~GLTexture2D()
	{
		if (m_textureID == 0) return;
		glDeleteTextures(1, &m_textureID);
		m_textureID = 0;
		m_target = 0;
	}

	GLTexture2DUPtr GLTexture2D::Create(ktxTexture* texture)
	{
		GLTexture2DUPtr glTexture(new GLTexture2D());
		if (!glTexture->Init(texture)) return nullptr;
		return glTexture;
	}

	bool GLTexture2D::Init(ktxTexture* texture)
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

	void GLTexture2D::Bind(uint32 textureUnit) const
	{
		if (!IsValid()) return;
		glBindTextureUnit(textureUnit, m_textureID);
	}

	void GLTexture2D::Unbind(uint32 textureUnit)
	{
		glBindTextureUnit(textureUnit, 0);
	}

	uint32 GLTexture2D::GetTextureID() const { return m_textureID; }
	GLenum GLTexture2D::GetTarget() const { return m_target; }
	bool GLTexture2D::IsValid() const { return m_textureID != 0 && m_target == GL_TEXTURE_2D; }
}
