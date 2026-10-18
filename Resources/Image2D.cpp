#include "pch.h"
#include "Image2D.h"
#include <Core/GLTexture2D.h>
#include <ktx/ktx.h>

namespace bdt
{
    Image2D::Image2D() = default;
    Image2D::~Image2D() = default;

    Image2DPtr Image2D::Create(const String& path)
    {
        Image2DPtr image(new Image2D());
        if (!image->Init(path)) return nullptr;
        return image;
    }

    bool Image2D::Init(const String& path)
    {
        if (path.empty()) return false;

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

        ktxTexture2* texture2 = reinterpret_cast<ktxTexture2*>(texture);

        // Basis Universal 텍스처인 경우 별도 처리
        if (ktxTexture2_NeedsTranscoding(texture2))
        {
            // 텍스처 용도에 따른 포맷 선택이 필요
            ktxTexture_Destroy(texture);
            return false;
        }

        // 2. OpenGL Texture 생성 및 업로드
        GLTexture2DUPtr glTexture = GLTexture2D::Create(texture);

        // 3. 임시 KTX 데이터 해제
        ktxTexture_Destroy(texture);

        if (!glTexture) return false;

        // 4. GPU 리소스 저장
        m_texture = std::move(glTexture);
        m_path = path;

        return true;
    }

    void Image2D::Bind(uint32 textureUnit) const
    {
        if (!IsValid()) return;
        m_texture->Bind(textureUnit);
    }

    bool Image2D::IsValid() const
    {
        return m_texture && m_texture->IsValid();
    }

    GLTexture2D* Image2D::GetTexture() const
    {
        return m_texture.get();
    }

    const String& Image2D::GetPath() const
    {
        return m_path;
    }
}