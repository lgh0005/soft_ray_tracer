#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <glad/gl.h>

namespace bdt
{
    BDT_CLASS_PTR(RenderTexture2D)

    class RenderTexture2D
    {
        BDT_DISABLE_COPY(RenderTexture2D)
        BDT_DISABLE_MOVE(RenderTexture2D)

    public:
        ~RenderTexture2D();
        static RenderTexture2DUPtr Create
        (
            uint32 width,
            uint32 height,
            GLenum format = GL_RGBA8,
            uint32 mipLevels = 1
        );

        bool Resize(uint32 width, uint32 height);
        void Bind(uint32 slot) const;
        static void Unbind(uint32 slot);

        uint32 GetTextureID() const;
        uint32 GetWidth() const;
        uint32 GetHeight() const;
        GLenum GetFormat() const;
        uint32 GetMipLevels() const;
        bool IsValid() const;

    private:
        RenderTexture2D();
        bool Init
        (
            uint32 width,
            uint32 height,
            GLenum format,
            uint32 mipLevels
        );

    private:
        uint32 m_textureID{ 0 };
        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
        GLenum m_format{ GL_RGBA8 };
        uint32 m_mipLevels{ 1 };
    };
}
