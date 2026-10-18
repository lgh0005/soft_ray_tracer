#pragma once
#include <Resources/Enums.h>
#include "Uniforms.h"

namespace bdt
{
    BDT_CLASS_PTR(Image2D)
    BDT_CLASS_PTR(Material)
    BDT_CLASS_PTR(GLBuffer)

    class RenderMaterial
    {
        BDT_DISABLE_COPY(RenderMaterial)
        BDT_DISABLE_MOVE(RenderMaterial)

    public:
        RenderMaterial();
        ~RenderMaterial();

        void Bind(const Material* material);
        static void Unbind();

    private:
        static void BindTexture
        (
            const ResourceHandle<Image2D>& texture,
            EMaterialTextureSlot slot,
            MaterialData& data
        );

    private:
        GLBufferUPtr m_materialUBO{ nullptr };
    };
}

