#include "pch.h"
#include "MyWorld.h"
#include <Resources/Resource.h>
#include <Core/Logger.h>

namespace bdt
{
	MyWorld::MyWorld(const String& tag) : Super(tag) { }
	MyWorld::~MyWorld() = default;

	MyWorldUPtr MyWorld::Create(const String& tag)
	{
		MyWorldUPtr world(new MyWorld(tag));
		if (!world->Init()) return nullptr;
		return world;
	}

	bool MyWorld::Init()
	{
		// TODO: MyWorld 고유의 초기 설정과 검증을 추가한다.
		// World::Init()은 창/GL 컨텍스트 생성 후 WorldManager에서 호출한다.
		return true;
	}

	void MyWorld::LoadResources()
	{
		// [TEMP] 실행 작업 디렉터리의 Assets를 ResourceManager로 로드한다.
		m_alienSoldierPath = "Assets/ailensoldier/ailensoldier.fbx";
		m_alienSoldier = BDT_RESOURCE.LoadModel(m_alienSoldierPath);
		m_loadResultReported = false;
		BDT_LOG_INFO("[ModelLoad] Requested: '{}'", m_alienSoldierPath);

		// [TEMP] 실행 작업 디렉터리의 Assets를 ResourceManager로 로드한다.
		m_swatGuyPath = "Assets/swat_guy/swat_guy.fbx";
		m_swatGuy = BDT_RESOURCE.LoadModel(m_swatGuyPath);
		m_loadResultReported2 = false;
		BDT_LOG_INFO("[ModelLoad] Requested: '{}'", m_swatGuyPath);

		// [TEMP] Animation 로딩 요청 #1
		m_idleAnimationPath = "Assets/ailensoldier/Idle.fbx";
		m_idleAnimation = BDT_RESOURCE.LoadAnimation(m_idleAnimationPath);
		m_animationResultReported = false;
		BDT_LOG_INFO("[AnimLoad] Requested: '{}'", m_idleAnimationPath);

		// [TEMP] Animation 로딩 요청 #2
		m_idleAnimationPath2 = "Assets/ailensoldier/walk.fbx";
		m_idleAnimation2 = BDT_RESOURCE.LoadAnimation(m_idleAnimationPath2);
		m_animationResultReported2 = false;
		BDT_LOG_INFO("[AnimLoad] Requested: '{}'", m_idleAnimationPath2);

		// [TEMP] Animation 로딩 요청 #3
		m_idleAnimationPath3 = "Assets/swat_guy/Chicken Dance.fbx";
		m_idleAnimation3 = BDT_RESOURCE.LoadAnimation(m_idleAnimationPath3);
		m_animationResultReported3 = false;
		BDT_LOG_INFO("[AnimLoad] Requested: '{}'", m_idleAnimationPath3);
	}

	void MyWorld::Update(float deltaTime)
	{
		Super::Update(deltaTime);

		// Model 로딩 완료 확인 #1
		if (!m_loadResultReported)
		{
			ModelPtr model = m_alienSoldier.Get();
			if (model)
			{
				BDT_LOG_INFO("[ModelLoad] Completed: '{}'", m_alienSoldierPath);
				m_loadResultReported = true;
			}
		}

		// Model 로딩 완료 확인 #2
		if (!m_loadResultReported2)
		{
			ModelPtr model = m_swatGuy.Get();
			if (model)
			{
				BDT_LOG_INFO("[ModelLoad] Completed: '{}'", m_swatGuyPath);
				m_loadResultReported2 = true;
			}
		}

		// Animation 로딩 완료 확인
		if (!m_animationResultReported)
		{
			AnimationPtr animation = m_idleAnimation.Get();
			if (animation)
			{
				const AnimationData& data = animation->GetData();
				BDT_LOG_INFO
				(
					"[AnimLoad] Completed: '{}', Name: '{}', Duration: {}, TPS: {}, Channels: {}",
					m_idleAnimationPath,
					animation->GetName(),
					animation->GetDuration(),
					animation->GetTicksPerSecond(),
					data.channels.size()
				);
				m_animationResultReported = true;
			}
		}

		// Animation 로딩 완료 확인
		if (!m_animationResultReported2)
		{
			AnimationPtr animation = m_idleAnimation2.Get();
			if (animation)
			{
				const AnimationData& data = animation->GetData();
				BDT_LOG_INFO
				(
					"[AnimLoad] Completed: '{}', Name: '{}', Duration: {}, TPS: {}, Channels: {}",
					m_idleAnimationPath2,
					animation->GetName(),
					animation->GetDuration(),
					animation->GetTicksPerSecond(),
					data.channels.size()
				);
				m_animationResultReported2 = true;
			}
		}

		// Animation 로딩 완료 확인
		if (!m_animationResultReported3)
		{
			AnimationPtr animation = m_idleAnimation3.Get();
			if (animation)
			{
				const AnimationData& data = animation->GetData();
				BDT_LOG_INFO
				(
					"[AnimLoad] Completed: '{}', Name: '{}', Duration: {}, TPS: {}, Channels: {}",
					m_idleAnimationPath3,
					animation->GetName(),
					animation->GetDuration(),
					animation->GetTicksPerSecond(),
					data.channels.size()
				);
				m_animationResultReported3 = true;
			}
		}
	}

	void MyWorld::CreateActors()
	{
		// [TEMP 예시 씬 설정]

		// 1. 메인 카메라 설정
		CameraComponent* camera = GetMainCamera()->GetCameraComponent();
		camera->SetPosition(vec3(0.0f, 1.0f, 4.0f));
		camera->SetRotation(quat(1.0f, 0.0f, 0.0f, 0.0f));
		camera->SetFOV(45.0f);
		camera->SetNearPlane(0.01f);
		camera->SetFarPlane(1000.0f);
		camera->SetViewportSize((uint32)BDT_WINDOW.GetFramebufferWidth(), (uint32)BDT_WINDOW.GetFramebufferHeight());

		// 2. 모델 액터 생성 #1
		SkinnedModelActor* soldier = AddSkinnedModelActor("AlienSoldier", m_alienSoldier);
		SkinnedModelRenderer* renderer = soldier->GetSkinnedModelRenderer();
		renderer->SetPosition(vec3(0.0f));
		renderer->SetRotation(quat(1.0f, 0.0f, 0.0f, 0.0f));
		renderer->SetScale(vec3(0.01f));

		// 2-1. Idle 애니메이션 반복 재생 요청
		AnimController& controller = soldier->GetAnimComponent()->GetController();
		controller.AddState("Idle", m_idleAnimation, true, 1.0f);
		controller.AddState("Walk", m_idleAnimation2, true, 1.0f);
		controller.Play("Idle");

		// 3. 모델 액터 생성 #2
		SkinnedModelActor* swat = AddSkinnedModelActor("SwatGuy", m_swatGuy);
		SkinnedModelRenderer* renderer2 = swat->GetSkinnedModelRenderer();
		renderer2->SetPosition(vec3(1.0f, 0.0f, 0.0f));
		renderer2->SetRotation(quat(1.0f, 0.0f, 0.0f, 0.0f));
		renderer2->SetScale(vec3(0.01f));

		// 3-1. Idle 애니메이션 반복 재생 요청 #2
		AnimController& controller2 = swat->GetAnimComponent()->GetController();
		controller2.AddState("Dance", m_idleAnimation3, true, 1.0f);
		controller2.Play("Dance");

		// 4. 방향광 설정
		DirectionalLightActor* sun = AddDirectionalLightActor("Sun");
		DirectionalLight* light = sun->GetDirectionalLight();
		light->SetColor(vec3(1.0f));
		light->SetIntensity(3.0f);
		light->SetEulerAngles(glm::radians(vec3(35.0f, -30.0f, 0.0f)));
	}
}
