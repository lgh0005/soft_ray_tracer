#pragma once
#include "RenderBatch.h"
#include "Instances.h"

namespace bdt
{
    class StaticRenderBatch : public RenderBatch
    {
        using Super = RenderBatch;

    public:
        StaticRenderBatch(Mesh* mesh, Material* material);
        ~StaticRenderBatch() override;

        void Clear() override;
        uint32 GetInstanceCount() const override;
        const List<StaticInstanceProperty>& GetProperties() const;
        void Add(const StaticInstanceProperty& property);

    private:
        List<StaticInstanceProperty> m_properties;
    };
}
