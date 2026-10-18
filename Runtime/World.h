#pragma once

namespace bdt
{
	BDT_CLASS_PTR(Actor)
	BDT_CLASS_PTR(CameraActor)
	BDT_CLASS_PTR(StaticModelActor)
	BDT_CLASS_PTR(SkinnedModelActor)
	BDT_CLASS_PTR(PrimitiveActor)
	BDT_CLASS_PTR(PointLightActor)
	BDT_CLASS_PTR(SpotLightActor)
	BDT_CLASS_PTR(DirectionalLightActor)
	BDT_CLASS_PTR(World)
		
	class World
	{
		BDT_DISABLE_COPY(World)
		friend class Actor;

	public:
		virtual ~World();

	public:
		void Init();
		virtual void Update(float deltaTime);
		virtual void Submit(RenderScene& scene) const;
		void Resize(uint32 width, uint32 height) const;
		void Clear();

		const String& GetTag() const;
		CameraActor* GetMainCamera() const;

	protected:
		explicit World(const String& tag);
		virtual void LoadResources();
		virtual void CreateActors();
		virtual void LoadEditorPannels();

	protected:
		template<typename... TArgs>
		StaticModelActor* AddStaticModelActor(TArgs&&... args);

		template<typename... TArgs>
		SkinnedModelActor* AddSkinnedModelActor(TArgs&&... args);

		template<typename... TArgs>
		PrimitiveActor* AddPrimitiveActor(TArgs&&... args);

		template<typename... TArgs>
		PointLightActor* AddPointLightActor(TArgs&&... args);

		template<typename... TArgs>
		SpotLightActor* AddSpotLightActor(TArgs&&... args);

		template<typename... TArgs>
		DirectionalLightActor* AddDirectionalLightActor(TArgs&&... args);

	protected:
		CameraActorUPtr m_mainCamera{ nullptr };

		// Actor 소유권
		List<StaticModelActorUPtr> m_staticModelActors;
		List<SkinnedModelActorUPtr> m_skinnedModelActors;
		List<PrimitiveActorUPtr> m_staticMeshActors;

		List<PointLightActorUPtr> m_pointLightActors;
		List<SpotLightActorUPtr> m_spotLightActors;
		List<DirectionalLightActorUPtr> m_directionalLightActors;

	private:
		String m_tag;

		// 전체 비소유 Actor 참조
		List<Actor*> m_actors;
	};
}

#include "World.inl"