#pragma once
#include <Core/GLStorageBuffer.h>
#include "RenderMaterial.h"
#include "Instances.h"

namespace bdt
{
    BDT_CLASS_PTR(GraphicsShader)
    BDT_CLASS_PTR(StaticRenderBatch)
    BDT_CLASS_PTR(StaticRenderQueue)

    class StaticRenderQueue
    {
        BDT_DISABLE_COPY(StaticRenderQueue)
        BDT_DISABLE_MOVE(StaticRenderQueue)

    public:
        ~StaticRenderQueue();
        static StaticRenderQueueUPtr Create();
        void Clear();
        void Add
        (
            Mesh* mesh,
            Material* material,
            const StaticInstanceProperty& property
        );

    public:
        bool Prepare();
        void Execute(GraphicsShader* shader);
        
        bool PrepareShadow(const List<ShadowData>& shadows);
        void ExecuteShadow(GraphicsShader* shader);

    private:
        StaticRenderQueue();
        bool Init();

        Dictionary<Mesh*, Dictionary<Material*, StaticRenderBatch>> m_batches;
        RenderMaterial m_renderMaterial;

        List<StaticInstanceProperty> m_properties;
        GLStorageBufferUPtr m_propertyBuffer;
        List<ShadowInstanceProperty> m_shadowProperties;
        GLStorageBufferUPtr m_shadowPropertyBuffer;
    };
}
