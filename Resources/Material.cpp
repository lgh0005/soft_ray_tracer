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

    void Material::SetRoughnessFactor(float factor)
    {
        m_desc.roughnessFactor = glm::clamp(factor, 0.0f, 1.0f);
    }

    void Material::SetEmissionStrength(float strength)
    {
        m_desc.emissionStrength = glm::max(strength, 0.0f);
    }
}
