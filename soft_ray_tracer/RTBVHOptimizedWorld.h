#pragma once
#include <Runtime/World.h>
#include <Resources/Material.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
	BDT_CLASS_PTR(RTBVHOptimizedWorld)

	class RTBVHOptimizedWorld : public World
	{
		using Super = World;

	public:
		~RTBVHOptimizedWorld() override;
		static RTBVHOptimizedWorldUPtr Create(const String& tag = "RTBVHOptimizedWorld");
		void Update(float deltaTime) override;

	protected:
		void LoadResources() override;
		void CreateActors() override;

	private:
		explicit RTBVHOptimizedWorld(const String& tag);
		bool Init();

		ResourceHandle<Material> m_platformMaterial;
		ResourceHandle<Material> m_sphereMaterial;
		ResourceHandle<Material> m_mirrorMaterial;
		ResourceHandle<Material> m_glassMaterial;
	};
}
