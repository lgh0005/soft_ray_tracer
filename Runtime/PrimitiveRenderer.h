#pragma once
#include "RootComponent.h"
#include "Enums.h"
#include <Rendering/RenderObject.h>

namespace bdt
{
	BDT_CLASS_PTR(StaticMesh)
	BDT_CLASS_PTR(PrimitiveRenderer)

	class PrimitiveRenderer : public RootComponent
	{
		BDT_DISABLE_COPY(PrimitiveRenderer)
		BDT_DISABLE_MOVE(PrimitiveRenderer)
		using Super = RootComponent;

	public:
		~PrimitiveRenderer() override;
		virtual EPrimitiveType GetPrimitiveType() const = 0;

		// 유효한 GL 컨텍스트에서 처음 조회할 때 메시를 생성한다.
		const List<RenderObject>& GetRenderObjects();
		const StaticMeshPtr& GetMesh() const;
		const ResourceHandle<Material>& GetMaterial() const;
		void SetMaterial(const ResourceHandle<Material>& material);

	protected:
		explicit PrimitiveRenderer(Actor* owner, const ResourceHandle<Material>& material = {});
		virtual StaticMeshPtr CreateMesh() const = 0;

	private:
		StaticMeshPtr m_mesh{ nullptr };
		ResourceHandle<Material> m_material;
		List<RenderObject> m_renderObjects;
	};
}
