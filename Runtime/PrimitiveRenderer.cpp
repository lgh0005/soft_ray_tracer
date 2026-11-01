#include "pch.h"
#include "PrimitiveRenderer.h"
#include <Resources/StaticMesh.h>

namespace bdt
{
	PrimitiveRenderer::PrimitiveRenderer(Actor* owner, const ResourceHandle<Material>& material)
		: Super(owner), m_material(material) { }
	PrimitiveRenderer::~PrimitiveRenderer() = default;

	const List<RenderObject>& PrimitiveRenderer::GetRenderObjects()
	{
		// 실패하면 빈 목록을 유지하고 다음 조회에서 다시 생성한다.
		if (!m_mesh) m_mesh = CreateMesh();
		if (!m_mesh) return m_renderObjects;

		const mat4 world = GetTransformMatrix();
		if (m_renderObjects.empty()) m_renderObjects.emplace_back(m_mesh, m_material, world);
		else m_renderObjects.front().SetWorldMatrix(world);
		return m_renderObjects;
	}

	const StaticMeshPtr& PrimitiveRenderer::GetMesh() const { return m_mesh; }
	const ResourceHandle<Material>& PrimitiveRenderer::GetMaterial() const { return m_material; }

	void PrimitiveRenderer::SetMaterial(const ResourceHandle<Material>& material)
	{
		m_material = material;
		m_renderObjects.clear();
	}
}
