#pragma once

namespace bdt
{
	BDT_CLASS_PTR(Mesh)
	BDT_CLASS_PTR(Material)

	class RenderBatch
	{
	public:
		RenderBatch(Mesh* mesh, Material* material);
		virtual ~RenderBatch();

		virtual void Clear() = 0;
		virtual uint32 GetInstanceCount() const = 0;

		Mesh* GetMesh() const;
		Material* GetMaterial() const;

	protected:
		Mesh* m_mesh{ nullptr };
		Material* m_material{ nullptr };
	};
}
