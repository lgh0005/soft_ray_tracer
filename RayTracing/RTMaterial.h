#pragma once
#include "RTGeometry.h"
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class Material;
    class Image2D;
    BDT_CLASS_PTR(RTMaterial)

    class RTMaterial
    {
        BDT_DISABLE_COPY(RTMaterial)

    public:
        explicit RTMaterial(const ResourceHandle<Material>& material = {});
        ~RTMaterial();
        RTMaterial(RTMaterial&&) noexcept = default;
        RTMaterial& operator=(RTMaterial&&) noexcept = default;

        bool Update(List<ResourceHandle<Image2D>>& textures);
        void Bind() const;
        const GPURTMaterial& GetData() const;
        const ResourceHandle<Material>& GetMaterial() const;

    private:
        static void BindTexture(const ResourceHandle<Image2D>& texture, uint32 slot);

        static uint32 AssignTextureSlot
        (
            const ResourceHandle<Image2D>& texture,
            List<ResourceHandle<Image2D>>& textures
        );

        ResourceHandle<Material> m_material;
        GPURTMaterial m_data{};
    };
}

