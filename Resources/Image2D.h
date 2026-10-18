#pragma once

struct ktxTexture;

namespace bdt
{
    BDT_CLASS_PTR(GLTexture2D)
    BDT_CLASS_PTR(Image2D)

    class Image2D
    {
        BDT_DISABLE_COPY(Image2D)
        BDT_DISABLE_MOVE(Image2D)

    public:
        ~Image2D();
        static Image2DPtr Create(const String& path);
        void Bind(uint32 textureUnit) const;
        bool IsValid() const;

    public:
        GLTexture2D* GetTexture() const;
        const String& GetPath() const;

    private:
        Image2D();
        bool Init(const String& path);

        String m_path;
        GLTexture2DUPtr m_texture{ nullptr };
    };
}