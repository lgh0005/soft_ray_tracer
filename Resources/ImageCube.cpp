#include "pch.h"
#include "ImageCube.h"
#include <Core/GLTextureCube.h>
#include <ktx/ktx.h>

namespace bdt
{
    ImageCube::ImageCube() = default;
    ImageCube::~ImageCube() = default;

    ImageCubePtr ImageCube::Create(const String& path)
    {
        ImageCubePtr image(new ImageCube());
        if (!image->Init(path)) return nullptr;
        return image;
    }

    bool ImageCube::Init(const String& path)
    {
        // 1. KTX 파일 로드
        ktxTexture* texture = nullptr;

        KTX_error_code result = ktxTexture_CreateFromNamedFile
        (
            path.c_str(),
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &texture
        );

        if (result != KTX_SUCCESS)
        {
            if (texture) ktxTexture_Destroy(texture);
            return false;
        }
        
        // KTX2만 허용
        if (texture->classId != ktxTexture2_c)
        {
            ktxTexture_Destroy(texture);
            return false;
        }

        // 2. OpenGL Cubemap 생성 및 업로드
        GLTextureCubeUPtr glTexture = GLTextureCube::Create(texture);

        // 3. 임시 KTX 데이터 해제
        ktxTexture_Destroy(texture);

        if (!glTexture) return false;

        // 4. GPU 리소스 저장
        m_texture = std::move(glTexture);
        m_path = path;

        return true;
    }

    void ImageCube::Bind(uint32 textureUnit) const
    {
        if (!IsValid()) return;
        m_texture->Bind(textureUnit);
    }

    bool ImageCube::IsValid() const
    {
        return m_texture && m_texture->IsValid();
    }

    GLTextureCube* ImageCube::GetTexture() const
    {
        return m_texture.get();
    }

    const String& ImageCube::GetPath() const
    {
        return m_path;
    }
}