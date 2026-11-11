#pragma once
#include <Runtime/World.h>
#include <Resources/Material.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
	BDT_CLASS_PTR(RTBruteForceWorld)

	class RTBruteForceWorld : public World
	{
		using Super = World;

	public:
		~RTBruteForceWorld() override;
		static RTBruteForceWorldUPtr Create(const String& tag = "RTBruteForceWorld");
		void Update(float deltaTime) override;

	protected:
		void LoadResources() override;
		void CreateActors() override;

	private:
		explicit RTBruteForceWorld(const String& tag);
		bool Init();

		ResourceHandle<Material> m_platformMaterial;
		ResourceHandle<Material> m_sphereMaterial;
		ResourceHandle<Material> m_mirrorMaterial;
		ResourceHandle<Material> m_glassMaterial;
	};
}
