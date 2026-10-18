#include "pch.h"
#include "RenderTexture2D.h"

namespace bdt
{
    RenderTexture2D::RenderTexture2D() = default;

    RenderTexture2D::~RenderTexture2D()
    {
        if (m_textureID != 0)
        {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }
    }

    RenderTexture2DUPtr RenderTexture2D::Create
    (
        uint32 width,
        uint32 height,
        GLenum format,
        uint32 mipLevels
    )
    {
        RenderTexture2DUPtr texture(new RenderTexture2D());
        if (!texture->Init(width, height, format, mipLevels)) return nullptr;
        return texture;
    }

    bool RenderTexture2D::Init
    (
        uint32 width,
        uint32 height,
        GLenum format,
        uint32 mipLevels
    )
    {
        if (width == 0 || height == 0 || mipLevels == 0)
            return false;

        // Mip Level 최대 개수 계산
        uint32 maxMipLevels = 1;
        for (uint32 dimension = glm::max(width, height); dimension > 1; dimension >>= 1)
            ++maxMipLevels;

        if (mipLevels > maxMipLevels)
            return false;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
        if (m_textureID == 0) return false;

        glTextureStorage2D
        (
            m_textureID,
            static_cast<GLsizei>(mipLevels),
            format,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height)
        );

        m_width = width;
        m_height = height;
        m_format = format;
        m_mipLevels = mipLevels;

        return true;
    }

    bool RenderTexture2D::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0)
            return false;

        if (width == m_width && height == m_height)
            return true;

        // 기존 텍스처를 유지한 채 새 텍스처 생성
        RenderTexture2DUPtr texture = Create(width, height, m_format, m_mipLevels);

        if (!texture)
            return false;

        // 새로운 Texture ID만 가져와 기존 객체에 적용
        glDeleteTextures(1, &m_textureID);

        m_textureID = texture->m_textureID;
        texture->m_textureID = 0;

        m_width = width;
        m_height = height;

        return true;
    }

    void RenderTexture2D::Bind(uint32 slot) const
    {
        if (!IsValid()) return;

        glBindTextureUnit(slot, m_textureID);
    }

    void RenderTexture2D::Unbind(uint32 slot)
    {
        glBindTextureUnit(slot, 0);
    }

    uint32 RenderTexture2D::GetTextureID() const
    {
        return m_textureID;
    }

    uint32 RenderTexture2D::GetWidth() const
    {
        return m_width;
    }

    uint32 RenderTexture2D::GetHeight() const
    {
        return m_height;
    }

    GLenum RenderTexture2D::GetFormat() const
    {
        return m_format;
    }

    uint32 RenderTexture2D::GetMipLevels() const
    {
        return m_mipLevels;
    }

    bool RenderTexture2D::IsValid() const
    {
        return m_textureID != 0;
    }
}
