#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <glad/gl.h>

namespace bdt
{
    BDT_CLASS_PTR(RenderTextureCube)

    class RenderTextureCube
    {
        BDT_DISABLE_COPY(RenderTextureCube)
        BDT_DISABLE_MOVE(RenderTextureCube)

    public:
        ~RenderTextureCube();
        static RenderTextureCubeUPtr Create
        (
            uint32 size,
            GLenum format = GL_RGBA16F,
            uint32 mipLevels = 1
        );

        bool Resize(uint32 size);

        void Bind(uint32 slot) const;
        static void Unbind(uint32 slot);

        uint32 GetTextureID() const;
        uint32 GetSize() const;
        GLenum GetFormat() const;
        uint32 GetMipLevels() const;

        bool IsValid() const;

    private:
        RenderTextureCube();
        bool Init
        (
            uint32 size,
            GLenum format = GL_RGBA16F,
            uint32 mipLevels = 1
        );

        uint32 m_textureID{ 0 };
        uint32 m_size{ 0 };
        GLenum m_format{ GL_RGBA16F };
        uint32 m_mipLevels{ 1 };
    };
}