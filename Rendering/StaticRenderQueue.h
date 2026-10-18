#pragma once
#include "InstanceBuffer.h"
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
        void Execute(GraphicsShader* shader);
        void Add
        (
            Mesh* mesh,
            Material* material,
            const StaticInstanceProperty& property
        );

    private:
        StaticRenderQueue();
        bool Init();

        Dictionary<Mesh*, Dictionary<Material*, StaticRenderBatch>> m_batches;
        RenderMaterial m_renderMaterial;
        InstanceBufferUPtr m_propertyBuffer;
    };
}
