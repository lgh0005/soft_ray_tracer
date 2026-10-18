#pragma once

namespace bdt
{
	BDT_CLASS_PTR(Model)
	BDT_CLASS_PTR(MyWorld)

	class MyWorld : public World
	{
		using Super = World;

	public:
		~MyWorld() override;
		static MyWorldUPtr Create(const String& tag = "MyWorld");
		void Update(float deltaTime) override;

	protected:
		void LoadResources() override;
		void CreateActors() override;

	private:
		explicit MyWorld(const String& tag);
		bool Init();

	private:

		// [TEMP] 모델 로드 테스트 #1
		ResourceHandle<Model> m_alienSoldier;
		String m_alienSoldierPath;
		bool m_loadResultReported{ true };

		// [TEMP] 모델 로드 테스트 #2
		ResourceHandle<Model> m_swatGuy;
		String m_swatGuyPath;
		bool m_loadResultReported2{ true };

		// [TEMP] Animation 로딩 테스트 #1
		ResourceHandle<Animation> m_idleAnimation;
		String m_idleAnimationPath;
		bool m_animationResultReported{ true };

		// [TEMP] Animation 로딩 테스트 #2
		ResourceHandle<Animation> m_idleAnimation2;
		String m_idleAnimationPath2;
		bool m_animationResultReported2{ true };

		// [TEMP] Animation 로딩 테스트 #3
		ResourceHandle<Animation> m_idleAnimation3;
		String m_idleAnimationPath3;
		bool m_animationResultReported3{ true };
	};
}
