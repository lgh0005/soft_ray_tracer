#include "pch.h"
#include "MyWorld.h"
#include <Resources/Resource.h>

namespace bdt
{
	namespace
	{
		constexpr cstr EnvironmentImagePath = "Assets/Default/Images/ferndale_studio_12_4k.ktx2";
		constexpr cstr AlienSoldierModelPath = "Assets/ailensoldier2/aliensoldier.fbx";
		constexpr cstr SwatGuyModelPath = "Assets/swat_guy/swat_guy.fbx";
		constexpr cstr AlienIdlePath = "Assets/ailensoldier/Idle.fbx";
		constexpr cstr AlienWalkPath = "Assets/ailensoldier/walk.fbx";
		constexpr cstr SwatStandingIdlePath = "Assets/swat_guy/Standing Idle.fbx";
		constexpr cstr SwatDancePath = "Assets/swat_guy/Chicken Dance.fbx";
		constexpr float ModelEmissionStrength = 8.0f;
	}

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
		// 배경 이미지 설정
		SetEnvironmentImage(EnvironmentImagePath);

		// ResourceManager가 캐시를 소유하며 액터 생성 시 같은 경로로 핸들을 조회한다.
		BDT_RESOURCE.LoadModel(AlienSoldierModelPath);
		BDT_RESOURCE.LoadModel(SwatGuyModelPath);
		BDT_RESOURCE.LoadAnimation(AlienIdlePath);
		BDT_RESOURCE.LoadAnimation(AlienWalkPath);
		BDT_RESOURCE.LoadAnimation(SwatStandingIdlePath);
		BDT_RESOURCE.LoadAnimation(SwatDancePath);
	}

	void MyWorld::Update(float deltaTime)
	{
		Super::Update(deltaTime);

		// 비동기 준비 후 메인 스레드에서 HDR 발광 강도와 갑옷의 거칠기를 설정한다.
		const auto updateMaterials = [](cstr path, bool adjustRoughness)
		{
			const auto model = BDT_RESOURCE.LoadModel(path).Get();
			if (!model) return;
			for (const auto& handle : model->GetMaterials())
			{
				if (auto material = handle.Get())
				{
					if (adjustRoughness) material->SetRoughnessFactor(0.5f);
					material->SetEmissionStrength(ModelEmissionStrength);
				}
			}
		};
		updateMaterials(AlienSoldierModelPath, true);
		updateMaterials(SwatGuyModelPath, false);
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
		SkinnedModelActor* soldier = AddSkinnedModelActor("AlienSoldier", BDT_RESOURCE.LoadModel(AlienSoldierModelPath));
		SkinnedModelRenderer* renderer = soldier->GetSkinnedModelRenderer();
		renderer->SetPosition(vec3(0.0f));
		renderer->SetRotation(quat(1.0f, 0.0f, 0.0f, 0.0f));
		renderer->SetScale(vec3(0.01f));

		// 2-1. Idle 애니메이션 반복 재생 요청
		AnimController& controller = soldier->GetAnimComponent()->GetController();
		controller.AddState("Idle", BDT_RESOURCE.LoadAnimation(AlienIdlePath), true, 1.0f);
		controller.AddState("Walk", BDT_RESOURCE.LoadAnimation(AlienWalkPath), true, 1.0f);
		controller.Play("Idle");

		// 3. 모델 액터 생성 #2
		SkinnedModelActor* swat = AddSkinnedModelActor("SwatGuy", BDT_RESOURCE.LoadModel(SwatGuyModelPath));
		SkinnedModelRenderer* renderer2 = swat->GetSkinnedModelRenderer();
		renderer2->SetPosition(vec3(1.0f, 0.0f, 0.0f));
		renderer2->SetRotation(quat(1.0f, 0.0f, 0.0f, 0.0f));
		renderer2->SetScale(vec3(0.01f));

		// 3-1. Standing Idle을 기본 반복 재생하고 Dance는 전환용으로 유지한다.
		AnimController& controller2 = swat->GetAnimComponent()->GetController();
		controller2.AddState("StandingIdle", BDT_RESOURCE.LoadAnimation(SwatStandingIdlePath), true, 1.0f);
		controller2.AddState("Dance", BDT_RESOURCE.LoadAnimation(SwatDancePath), true, 1.0f);
		controller2.Play("StandingIdle");

		// 4. 윗면이 Y=0인 발판형 직육면체
		MaterialDesc platformDesc{};
		platformDesc.roughnessFactor = 0.85f;
		MaterialTextures platformTextures{};
		platformTextures.albedo = BDT_RESOURCE.LoadImage2D("Assets/Default/Images/sand-ground-textured.ktx2");
		auto platformMaterial = BDT_RESOURCE.LoadMaterial("MyWorld.Platform", platformDesc, platformTextures);
		auto* platform = AddPrimitiveActor("Platform", EPrimitiveType::Box, platformMaterial);
		auto* platformRenderer = platform->GetPrimitiveRenderer();
		platformRenderer->SetPosition(vec3(0.0f, -0.15f, 0.0f));
		platformRenderer->SetScale(vec3(5.0f, 0.3f, 3.0f));

		// 5. 발판 위에서 기존 모델 왼쪽에 놓인 구
		MaterialDesc sphereDesc{};
		sphereDesc.metallicFactor = 1.0f;
		sphereDesc.roughnessFactor = 1.0f;
		MaterialTextures sphereTextures{};
		sphereTextures.albedo = BDT_RESOURCE.LoadImage2D("Assets/MetalBall/worn-shiny-metal-albedo.ktx2");
		sphereTextures.normal = BDT_RESOURCE.LoadImage2D("Assets/MetalBall/worn-shiny-metal-Normal-ogl.ktx2");
		sphereTextures.ao = BDT_RESOURCE.LoadImage2D("Assets/MetalBall/worn-shiny-metal-ao.ktx2");
		sphereTextures.roughness = BDT_RESOURCE.LoadImage2D("Assets/MetalBall/worn-shiny-metal-Roughness.ktx2");
		sphereTextures.height = BDT_RESOURCE.LoadImage2D("Assets/MetalBall/worn-shiny-metal-Height.ktx2");
		sphereTextures.metallic = BDT_RESOURCE.LoadImage2D("Assets/MetalBall/worn-shiny-metal-Metallic.ktx2");
		auto sphereMaterial = BDT_RESOURCE.LoadMaterial("MyWorld.Sphere", sphereDesc, sphereTextures);
		auto* sphere = AddPrimitiveActor("Sphere", EPrimitiveType::Sphere, sphereMaterial);
		sphere->GetPrimitiveRenderer()->SetPosition(vec3(-1.25f, 0.5f, 0.0f));

		// 6. 방향광 설정
		DirectionalLightActor* sun = AddDirectionalLightActor("Sun");
		DirectionalLight* light = sun->GetDirectionalLight();
		light->SetColor(vec3(1.0f));
		light->SetIntensity(3.0f);
		light->SetShadowOrthoSize(vec2(5.0f, 5.0f));
		light->SetShadowEnabled(true);
		light->SetEulerAngles(glm::radians(vec3(35.0f, -30.0f, 0.0f)));
	}
}
