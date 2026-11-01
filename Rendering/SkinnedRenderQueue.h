#pragma once
#include <Core/GLStorageBuffer.h>
#include "RenderMaterial.h"
#include "Instances.h"

namespace bdt
{
    BDT_CLASS_PTR(GraphicsShader)
    BDT_CLASS_PTR(SkinnedRenderBatch)
    BDT_CLASS_PTR(SkinnedRenderQueue)

    class SkinnedRenderQueue
    {
    public:
        ~SkinnedRenderQueue();
        static SkinnedRenderQueueUPtr Create();
        void Clear();
        void Add
        (
            Mesh* mesh,
            Material* material,
            const SkinnedInstanceProperty& property,
            const List<mat4>& bones
        );

    public:
        bool Prepare();
        void Execute(GraphicsShader* shader);
        
        bool PrepareShadow(const List<ShadowData>& shadows);
        void ExecuteShadow(GraphicsShader* shader);

    private:
        SkinnedRenderQueue();
        bool Init();

        Dictionary<Mesh*, Dictionary<Material*, SkinnedRenderBatch>> m_batches;
        RenderMaterial m_renderMaterial;

        // Geometry Instance
        List<SkinnedInstanceProperty> m_properties;
        List<mat4> m_boneMatrices;
        GLStorageBufferUPtr m_propertyBuffer;
        GLStorageBufferUPtr m_boneBuffer;

        // Shadow Instance
        List<ShadowInstanceProperty> m_shadowProperties;
        GLStorageBufferUPtr m_shadowPropertyBuffer;
    };
}
