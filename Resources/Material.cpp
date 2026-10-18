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
}