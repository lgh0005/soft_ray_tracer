#pragma once
#include "RootComponent.h"
#include <Rendering/RenderObject.h>

namespace bdt
{
	BDT_CLASS_PTR(Model)

	class ModelRenderer : public RootComponent
	{
		using Super = RootComponent;

	public:
		virtual ~ModelRenderer() override;
		explicit ModelRenderer(Actor* owner,  const ResourceHandle<Model>& model);
		const List<RenderObject>& GetRenderObjects();
		ModelPtr GetModel() const;

	protected:
		virtual bool BuildRenderObjects();
		virtual void UpdateWorldTransforms();
		virtual void MarkTransformDirty() override;

	protected:
		ResourceHandle<Model> m_model;
		List<RenderObject> m_renderObjects;
		List<uint32> m_renderNodeIndices;
		bool m_renderObjectsDirty{ true };
	};
}
