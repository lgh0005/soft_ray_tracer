#include "pch.h"
#include "RenderTextureCube.h"
#include <Core/GLTextureCube.h>

namespace bdt
{
    RenderTextureCube::RenderTextureCube() = default;
    RenderTextureCube::~RenderTextureCube()
    {
        if (m_textureID != 0)
        {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }
    }

    RenderTextureCubeUPtr RenderTextureCube::Create(uint32 size, GLenum format, uint32 mipLevels)
    {
        RenderTextureCubeUPtr texture(new RenderTextureCube());
        if (!texture->Init(size, format, mipLevels)) return nullptr;
        return texture;
    }

    bool RenderTextureCube::Init(uint32 size, GLenum format, uint32 mipLevels)
    {
        if (size == 0 || mipLevels == 0)
            return false;

        // Mip Level 최대 개수 계산
        uint32 maxMipLevels = 1;
        for (uint32 dimension = size; dimension > 1; dimension >>= 1)
            ++maxMipLevels;

        if (mipLevels > maxMipLevels)
            return false;

        // Cubemap Texture 생성
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_textureID);

        if (m_textureID == 0)
            return false;

        // 6개 면의 Mipmap Storage 할당
        glTextureStorage2D
        (
            m_textureID,
            static_cast<GLsizei>(mipLevels),
            format,
            static_cast<GLsizei>(size),
            static_cast<GLsizei>(size)
        );

        m_size = size;
        m_format = format;
        m_mipLevels = mipLevels;

        return true;
    }

    bool RenderTextureCube::Resize(uint32 size)
    {
        if (size == 0) return false;
        if (size == m_size && IsValid()) return true;

        RenderTextureCubeUPtr texture = Create(size, m_format, m_mipLevels);
        if (!texture) return false;

        // 새 텍스처 생성 성공 후 기존 텍스처 해제
        glDeleteTextures(1, &m_textureID);

        m_textureID = texture->m_textureID;
        texture->m_textureID = 0;
        m_size = size;

        return true;
    }

    void RenderTextureCube::Bind(uint32 slot) const
    {
        if (!IsValid()) return;
        glBindTextureUnit(slot, m_textureID);
    }

    void RenderTextureCube::Unbind(uint32 slot)
    {
        glBindTextureUnit(slot, 0);
    }

    uint32 RenderTextureCube::GetTextureID() const
    {
        return m_textureID;
    }

    uint32 RenderTextureCube::GetSize() const
    {
        return m_size;
    }

    GLenum RenderTextureCube::GetFormat() const
    {
        return m_format;
    }

    uint32 RenderTextureCube::GetMipLevels() const
    {
        return m_mipLevels;
    }

    bool RenderTextureCube::IsValid() const
    {
        return m_textureID != 0;
    }
}