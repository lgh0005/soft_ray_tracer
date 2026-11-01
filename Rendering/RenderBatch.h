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

		uint32 GetBaseInstance() const;
		void SetBaseInstance(uint32 baseInstance);

		uint32 GetShadowBaseInstance() const;
		void SetShadowBaseInstance(uint32 baseInstance);
		uint32 GetShadowInstanceCount() const;
		void SetShadowInstanceCount(uint32 count);

	protected:
		Mesh* m_mesh{ nullptr };
		Material* m_material{ nullptr };

		uint32 m_baseInstance{ 0 };
		uint32 m_shadowBaseInstance{ 0 };
		uint32 m_shadowInstanceCount{ 0 };
	};
}
