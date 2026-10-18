#pragma once
#include "ModelRenderer.h"

namespace bdt
{
    BDT_CLASS_PTR(Model)

    class SkinnedModelRenderer : public ModelRenderer
    {
        using Super = ModelRenderer;

    public:
        virtual ~SkinnedModelRenderer() override;
        explicit SkinnedModelRenderer(Actor* owner, const ResourceHandle<Model>& model);
        bool ApplyPose(const List<mat4>& nodeGlobalTransforms);

    protected:
        virtual bool BuildRenderObjects() override;
        virtual void UpdateWorldTransforms() override;

    private:
        bool BuildBoneMatrices(const Model& model);
        bool UpdateBoneMatrices(const List<mat4>& nodeGlobalTransforms);

    private:
        List<List<mat4>> m_bonePalettes;
    };
}