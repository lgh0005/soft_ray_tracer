#pragma once

namespace bdt
{
    BDT_CLASS_PTR(RenderTextureArray2D)

    class RenderTextureArray2D
    {
        BDT_DISABLE_COPY(RenderTextureArray2D)
        BDT_DISABLE_MOVE(RenderTextureArray2D)

    public:
        ~RenderTextureArray2D();
        static RenderTextureArray2DUPtr Create
        (
            uint32 width,
            uint32 height,
            uint32 layers,
            GLenum format = GL_RGBA8,
            uint32 mipLevels = 1
        );

        bool Resize(uint32 width, uint32 height);
        bool Resize(uint32 width, uint32 height, uint32 layers);
        void Bind(uint32 slot) const;
        static void Unbind(uint32 slot);

        uint32 GetTextureID() const;
        uint32 GetWidth() const;
        uint32 GetHeight() const;
        uint32 GetLayers() const;
        GLenum GetFormat() const;
        uint32 GetMipLevels() const;
        bool IsValid() const;

    private:
        RenderTextureArray2D();
        bool Init
        (
            uint32 width,
            uint32 height,
            uint32 layers,
            GLenum format,
            uint32 mipLevels
        );

    private:
        uint32 m_textureID{ 0 };
        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
        uint32 m_layers{ 0 };
        GLenum m_format{ GL_RGBA8 };
        uint32 m_mipLevels{ 1 };
    };
}
