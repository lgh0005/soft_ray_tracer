#include "pch.h"
#include "RenderTextureArray2D.h"

namespace bdt
{
    RenderTextureArray2D::RenderTextureArray2D() = default;

    RenderTextureArray2D::~RenderTextureArray2D()
    {
        if (m_textureID != 0)
        {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }
    }

    RenderTextureArray2DUPtr RenderTextureArray2D::Create
    (
        uint32 width,
        uint32 height,
        uint32 layers,
        GLenum format,
        uint32 mipLevels
    )
    {
        RenderTextureArray2DUPtr texture(new RenderTextureArray2D());
        if (!texture->Init(width, height, layers, format, mipLevels)) return nullptr;
        return texture;
    }

    bool RenderTextureArray2D::Init
    (
        uint32 width,
        uint32 height,
        uint32 layers,
        GLenum format,
        uint32 mipLevels
    )
    {
        if (width == 0 || height == 0 || layers == 0 || mipLevels == 0)
            return false;

        // Mip Level 최대 개수 계산
        uint32 maxMipLevels = 1;
        for (uint32 dimension = glm::max(width, height); dimension > 1; dimension >>= 1)
            ++maxMipLevels;

        if (mipLevels > maxMipLevels)
            return false;

        GLint maxTextureSize = 0;
        GLint maxArrayLayers = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);
        if (maxTextureSize <= 0 || maxArrayLayers <= 0 ||
            width > static_cast<uint32>(maxTextureSize) ||
            height > static_cast<uint32>(maxTextureSize) ||
            layers > static_cast<uint32>(maxArrayLayers))
            return false;

        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_textureID);
        if (m_textureID == 0) return false;

        glTextureStorage3D
        (
            m_textureID,
            static_cast<GLsizei>(mipLevels),
            format,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            static_cast<GLsizei>(layers)
        );

        GLint immutableFormat = GL_FALSE;
        glGetTextureParameteriv(m_textureID, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
        if (immutableFormat != GL_TRUE) return false;

        m_width = width;
        m_height = height;
        m_layers = layers;
        m_format = format;
        m_mipLevels = mipLevels;

        return true;
    }

    bool RenderTextureArray2D::Resize(uint32 width, uint32 height)
    {
        return Resize(width, height, m_layers);
    }

    bool RenderTextureArray2D::Resize(uint32 width, uint32 height, uint32 layers)
    {
        if (width == 0 || height == 0 || layers == 0)
            return false;

        if (width == m_width && height == m_height && layers == m_layers && IsValid())
            return true;

        // 기존 텍스처를 유지한 채 새 텍스처 생성
        RenderTextureArray2DUPtr texture = Create(width, height, layers, m_format, m_mipLevels);

        if (!texture)
            return false;

        // 새로운 Texture ID만 가져와 기존 객체에 적용
        glDeleteTextures(1, &m_textureID);

        m_textureID = texture->m_textureID;
        texture->m_textureID = 0;

        m_width = width;
        m_height = height;
        m_layers = layers;

        return true;
    }

    void RenderTextureArray2D::Bind(uint32 slot) const
    {
        if (!IsValid()) return;

        glBindTextureUnit(slot, m_textureID);
    }

    void RenderTextureArray2D::Unbind(uint32 slot)
    {
        glBindTextureUnit(slot, 0);
    }

    uint32 RenderTextureArray2D::GetTextureID() const
    {
        return m_textureID;
    }

    uint32 RenderTextureArray2D::GetWidth() const
    {
        return m_width;
    }

    uint32 RenderTextureArray2D::GetHeight() const
    {
        return m_height;
    }

    uint32 RenderTextureArray2D::GetLayers() const
    {
        return m_layers;
    }

    GLenum RenderTextureArray2D::GetFormat() const
    {
        return m_format;
    }

    uint32 RenderTextureArray2D::GetMipLevels() const
    {
        return m_mipLevels;
    }

    bool RenderTextureArray2D::IsValid() const
    {
        return m_textureID != 0;
    }
}
