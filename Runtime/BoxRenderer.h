#pragma once
#include "PrimitiveRenderer.h"

namespace bdt
{
	BDT_CLASS_PTR(BoxRenderer)

	class BoxRenderer final : public PrimitiveRenderer
	{
		using Super = PrimitiveRenderer;

	public:
		explicit BoxRenderer(Actor* owner, const ResourceHandle<Material>& material = {});
		~BoxRenderer() override;
		EPrimitiveType GetPrimitiveType() const override;

	protected:
		StaticMeshPtr CreateMesh() const override;
	};
}
