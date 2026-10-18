#include "pch.h"
#include "RenderMaterial.h"
#include <Core/GLBuffer.h>
#include <Resources/Material.h>
#include <Resources/Image2D.h>
#include <Resources/Enums.h>
#include "Enums.h"

namespace bdt
{
    RenderMaterial::RenderMaterial()
    {
        m_materialUBO = GLBuffer::Create
        (
            sizeof(MaterialData),
            nullptr,
            GL_DYNAMIC_STORAGE_BIT
        );
    }

    RenderMaterial::~RenderMaterial() = default;

    void RenderMaterial::Bind(const Material* material)
    {
        if (!m_materialUBO->IsValid())
            return;

        MaterialData data;
        Unbind();

        if (material)
        {
            const MaterialDesc& desc = material->GetDesc();
            const MaterialTextures& textures = material->GetTextures();

            // Material 속성
            data.albedoFactor = desc.albedoFactor;
            data.emissiveFactor = vec4(desc.emissiveFactor, 0.0f);
            data.metallicFactor = desc.metallicFactor;
            data.roughnessFactor = desc.roughnessFactor;
            data.emissionStrength = desc.emissionStrength;
            data.heightScale = desc.heightScale;

            // Material Textures
            BindTexture(textures.albedo, EMaterialTextureSlot::Albedo, data);
            BindTexture(textures.normal, EMaterialTextureSlot::Normal, data);
            BindTexture(textures.ao, EMaterialTextureSlot::AO, data);
            BindTexture(textures.roughness, EMaterialTextureSlot::Roughness, data);
            BindTexture(textures.emission, EMaterialTextureSlot::Emission, data);
            BindTexture(textures.height, EMaterialTextureSlot::Height, data);
        }

        // Material UBO 데이터 갱신
        m_materialUBO->SetData
        (
            0,
            sizeof(MaterialData),
            &data
        );

        // Material UBO 바인딩
        m_materialUBO->BindBase
        (
            GL_UNIFORM_BUFFER, 
            static_cast<uint32>(EUniformBinding::Material)
        );
    }

    void RenderMaterial::Unbind()
    {
        // ubo 언바인딩
        glBindBufferBase
        (
            GL_UNIFORM_BUFFER, 
            static_cast<uint32>(EUniformBinding::Material), 
            0
        );

        // 텍스처 언바인딩
        for (uint32 slot = 0;
            slot < static_cast<uint32>(EMaterialTextureSlot::Count);
            ++slot)
        {
            glBindTextureUnit(slot, 0);
        }
    }

    void RenderMaterial::BindTexture
    (
        const ResourceHandle<Image2D>& texture,
        EMaterialTextureSlot slot,
        MaterialData& data
    )
    {
        Image2DPtr image = texture.Get();
        if (!image) return;
        if (!image->IsValid()) return;

        const uint32 index = static_cast<uint32>(slot);
        image->Bind(index);
        data.textureFlags |= (1u << index);
    }
}