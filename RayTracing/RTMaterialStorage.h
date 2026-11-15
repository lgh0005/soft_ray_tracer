#pragma once
#include "RTGeometry.h"
#include "RTMaterial.h"
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class Material;
    BDT_CLASS_PTR(GLStorageBuffer)
    BDT_CLASS_PTR(GLSampler)
    BDT_CLASS_PTR(RTMaterialStorage)

    class RTMaterialStorage
    {
        BDT_DISABLE_COPY(RTMaterialStorage)
        BDT_DISABLE_MOVE(RTMaterialStorage)

    public:
        ~RTMaterialStorage();
        static RTMaterialStorageUPtr Create();
        void Bind() const;
        bool Upload();
        void Clear();
        void Shutdown();

        uint32 RegisterMaterial(const ResourceHandle<Material>& material);
        uint32 GetMaterialCount() const;

    private:
        RTMaterialStorage();
        bool Init();
        void ResetMaterialIndices();

        GLStorageBufferUPtr m_materialBuffer;
        GLSamplerUPtr m_materialSampler;
        List<RTMaterial> m_materials;
        List<GPURTMaterial> m_gpuMaterials;
    };
}

