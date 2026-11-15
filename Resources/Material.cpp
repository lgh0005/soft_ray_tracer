#include "pch.h"
#include "Material.h"

namespace bdt
{
    Material::Material
    (
        const MaterialDesc& desc,
        const MaterialTextures& textures
    ) : m_desc(desc), m_textures(textures) { }

    Material::~Material() = default;

    MaterialPtr Material::Create
    (
        const MaterialDesc& desc,
        const MaterialTextures& textures
    )
    {
        return MaterialPtr(new Material(desc, textures));
    }

    const MaterialDesc& Material::GetDesc() const
    {
        return m_desc;
    }

    const MaterialTextures& Material::GetTextures() const
    {
        return m_textures;
    }

    uint32 Material::GetIndex() const
    {
        return m_index;
    }

    void Material::SetIndex(uint32 index)
    {
        m_index = index;
    }

    void Material::SetAlbedoFactor(const vec4& factor)
    {
        m_desc.albedoFactor = glm::clamp(factor, vec4(0.0f), vec4(1.0f));
    }

    void Material::SetEmissiveFactor(const vec3& factor)
    {
        m_desc.emissiveFactor = glm::max(factor, vec3(0.0f));
    }

    void Material::SetMetallicFactor(float factor)
    {
        m_desc.metallicFactor = glm::clamp(factor, 0.0f, 1.0f);
    }

    void Material::SetRoughnessFactor(float factor)
    {
        m_desc.roughnessFactor = glm::clamp(factor, 0.0f, 1.0f);
    }

    void Material::SetEmissionStrength(float strength)
    {
        m_desc.emissionStrength = glm::max(strength, 0.0f);
    }

    void Material::SetIOR(float ior)
    {
        m_desc.ior = ior;
    }

    void Material::SetTransmission(float transmission)
    {
        m_desc.transmission = glm::clamp(transmission, 0.0f, 1.0f);
    }
}
