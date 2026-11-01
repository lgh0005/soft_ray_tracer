#pragma once
#include "PrimitiveRenderer.h"

namespace bdt
{
	BDT_CLASS_PTR(SphereRenderer)

	class SphereRenderer final : public PrimitiveRenderer
	{
		using Super = PrimitiveRenderer;

	public:
		explicit SphereRenderer(Actor* owner, const ResourceHandle<Material>& material = {});
		~SphereRenderer() override;
		EPrimitiveType GetPrimitiveType() const override;

	protected:
		StaticMeshPtr CreateMesh() const override;
	};
}

