#pragma once

struct ktxTexture;

namespace bdt
{
    BDT_CLASS_PTR(GLTextureCube)
    BDT_CLASS_PTR(ImageCube)

    class ImageCube
    {
        BDT_DISABLE_COPY(ImageCube)
        BDT_DISABLE_MOVE(ImageCube)

    public:
        ~ImageCube();
        static ImageCubePtr Create(const String& path);
        void Bind(uint32 textureUnit) const;
        bool IsValid() const;

    public:
        GLTextureCube* GetTexture() const;
        const String& GetPath() const;

    private:
        ImageCube();
        bool Init(const String& path);

        String m_path;
        GLTextureCubeUPtr m_texture{ nullptr };
    };
}