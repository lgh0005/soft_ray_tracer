#pragma once
#include "RenderBatch.h"
#include "Instances.h"

namespace bdt
{
    class SkinnedRenderBatch : public RenderBatch
    {
        using Super = RenderBatch;

    public:
        SkinnedRenderBatch(Mesh* mesh, Material* material);
        ~SkinnedRenderBatch() override;

        void Clear() override;
        uint32 GetInstanceCount() const override;
        const List<SkinnedInstanceProperty>& GetProperties() const;
        const List<mat4>& GetBoneMatrices() const;
        void Add
        (
            const SkinnedInstanceProperty& property,
            const List<mat4>& bones
        );

    private:
        List<SkinnedInstanceProperty> m_properties;
        List<mat4> m_boneMatrices;
    };
}
