#pragma once
#include "MaterialData.h"

namespace bdt
{
    BDT_CLASS_PTR(Material)
    BDT_CLASS_PTR(Image2D)

	class Material
	{
		BDT_DISABLE_COPY(Material)
		BDT_DISABLE_MOVE(Material)

    public:
        ~Material();
        static MaterialPtr Create
        (
            const MaterialDesc& desc,
            const MaterialTextures& textures
        );

        // Material Data
        const MaterialDesc& GetDesc() const;
        const MaterialTextures& GetTextures() const;

    private:
        Material
        (
            const MaterialDesc& desc,
            const MaterialTextures& textures
        );

        MaterialDesc m_desc;
        MaterialTextures m_textures;
	};
}

