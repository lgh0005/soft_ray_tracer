#pragma once
#include "InstanceBuffer.h"
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
        void Execute(GraphicsShader* shader);
        void Add
        (
            Mesh* mesh,
            Material* material,
            const SkinnedInstanceProperty& property,
            const List<mat4>& bones
        );

    private:
        SkinnedRenderQueue();
        bool Init();

        Dictionary<Mesh*, Dictionary<Material*, SkinnedRenderBatch>> m_batches;
        RenderMaterial m_renderMaterial;

        InstanceBufferUPtr m_propertyBuffer;
        InstanceBufferUPtr m_boneBuffer;
    };
}
