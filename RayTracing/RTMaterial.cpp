#include "pch.h"
#include "RTMaterial.h"
#include <Resources/Material.h>
#include <Resources/Image2D.h>

namespace bdt
{
    RTMaterial::RTMaterial(const ResourceHandle<Material>& material) : m_material(material) { }
    RTMaterial::~RTMaterial() = default;

    bool RTMaterial::Update(List<ResourceHandle<Image2D>>& textures)
    {
        const auto material = m_material.Get();
        MaterialDesc desc{};
        if (material) desc = material->GetDesc();

        GPURTMaterial data{};
        data.albedoFactor = desc.albedoFactor;
        data.emission = vec4(desc.emissiveFactor * desc.emissionStrength, 0.0f);
        data.metallicFactor = glm::clamp(desc.metallicFactor, 0.0f, 1.0f);
        data.roughnessFactor = glm::clamp(desc.roughnessFactor, 0.0f, 1.0f);
        data.ior = desc.ior;
        data.transmission = glm::clamp(desc.transmission, 0.0f, 1.0f);

        if (material)
        {
            const MaterialTextures& materialTextures = material->GetTextures();
            data.albedoTextureIndex = AssignTextureSlot(materialTextures.albedo, textures);
            data.normalTextureIndex = AssignTextureSlot(materialTextures.normal, textures);
            data.aoTextureIndex = AssignTextureSlot(materialTextures.ao, textures);
            data.roughnessTextureIndex = AssignTextureSlot(materialTextures.roughness, textures);
            data.emissionTextureIndex = AssignTextureSlot(materialTextures.emission, textures);
            data.metallicTextureIndex = AssignTextureSlot(materialTextures.metallic, textures);
        }

        m_data = data;
        return true;
    }

    void RTMaterial::Bind() const
    {
        const auto material = m_material.Get();
        if (!material) return;

        const MaterialTextures& textures = material->GetTextures();
        BindTexture(textures.albedo, m_data.albedoTextureIndex);
        BindTexture(textures.normal, m_data.normalTextureIndex);
        BindTexture(textures.ao, m_data.aoTextureIndex);
        BindTexture(textures.roughness, m_data.roughnessTextureIndex);
        BindTexture(textures.emission, m_data.emissionTextureIndex);
        BindTexture(textures.metallic, m_data.metallicTextureIndex);
    }

    void RTMaterial::BindTexture(const ResourceHandle<Image2D>& texture, uint32 slot)
    {
        if (slot >= 16u) return;
        const auto image = texture.Get();
        if (!image) return;
        if (!image->IsValid()) return;
        image->Bind(slot);
    }

    uint32 RTMaterial::AssignTextureSlot
    (
        const ResourceHandle<Image2D>& texture,
        List<ResourceHandle<Image2D>>& textures
    )
    {
        const auto image = texture.Get();
        if (!image) return MaxValue<uint32>;
        if (!image->IsValid()) return MaxValue<uint32>;

        for (uint32 i = 0; i < textures.size(); ++i)
            if (textures[i].Get() == image) return i;

        if (textures.size() >= 16u) return MaxValue<uint32>;
        const uint32 textureIndex = static_cast<uint32>(textures.size());
        textures.push_back(texture);
        return textureIndex;
    }

    const GPURTMaterial& RTMaterial::GetData() const
    {
        return m_data;
    }

    const ResourceHandle<Material>& RTMaterial::GetMaterial() const
    {
        return m_material;
    }
}
