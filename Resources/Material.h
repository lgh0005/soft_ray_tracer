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

        uint32 GetIndex() const;
        void SetIndex(uint32 index);

        void SetAlbedoFactor(const vec4& factor);
        void SetEmissiveFactor(const vec3& factor);
        void SetMetallicFactor(float factor);
        void SetRoughnessFactor(float factor);
        void SetEmissionStrength(float strength);
        void SetIOR(float ior);
        void SetTransmission(float transmission);

    private:
        Material
        (
            const MaterialDesc& desc,
            const MaterialTextures& textures
        );

        MaterialDesc m_desc;
        MaterialTextures m_textures;
        uint32 m_index{ MaxValue<uint32> };
	};
}

