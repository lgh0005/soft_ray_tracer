#include "pch.h"
#include "RTBruteForceWorld.h"
#include <Resources/Resource.h>

namespace bdt
{
	namespace
	{
		constexpr cstr EnvironmentImagePath = "Assets/Default/Images/ferndale_studio_12_4k.ktx2";
		constexpr cstr PlatformAlbedoPath = "Assets/Default/Images/sand-ground-textured.ktx2";
		constexpr cstr SphereAlbedoPath = "Assets/MetalBall/worn-shiny-metal-albedo.ktx2";
		constexpr cstr SphereNormalPath = "Assets/MetalBall/worn-shiny-metal-Normal-ogl.ktx2";
		constexpr cstr SphereAOPath = "Assets/MetalBall/worn-shiny-metal-ao.ktx2";
		constexpr cstr SphereRoughnessPath = "Assets/MetalBall/worn-shiny-metal-Roughness.ktx2";
		constexpr cstr SphereHeightPath = "Assets/MetalBall/worn-shiny-metal-Height.ktx2";
		constexpr cstr SphereMetallicPath = "Assets/MetalBall/worn-shiny-metal-Metallic.ktx2";

		constexpr float ModelEmissionStrength = 8.0f;
	}

	RTBruteForceWorld::RTBruteForceWorld(const String& tag) : Super(tag) { }
	RTBruteForceWorld::~RTBruteForceWorld() = default;

	RTBruteForceWorldUPtr RTBruteForceWorld::Create(const String& tag)
	{
		RTBruteForceWorldUPtr world(new RTBruteForceWorld(tag));
		if (!world->Init()) return nullptr;
		return world;
	}

	bool RTBruteForceWorld::Init()
	{
		// TODO: RTBruteForceWorld 고유의 초기 설정과 검증을 추가한다.
		// World::Init()은 창/GL 컨텍스트 생성 후 WorldManager에서 호출한다.
		return true;
	}

	void RTBruteForceWorld::LoadResources()
	{
		// 배경 이미지 설정
		SetEnvironmentImage(EnvironmentImagePath);

		// 발판 머티리얼과 텍스처 로드
		MaterialDesc platformDesc{};
		platformDesc.roughnessFactor = 0.85f;
		MaterialTextures platformTextures{};
		platformTextures.albedo = BDT_RESOURCE.LoadImage2D(PlatformAlbedoPath);
		m_platformMaterial = BDT_RESOURCE.LoadMaterial("RTBruteForceWorld.Platform", platformDesc, platformTextures);

		// 구 머티리얼과 텍스처 로드
		MaterialDesc sphereDesc{};
		sphereDesc.metallicFactor = 1.0f;
		sphereDesc.roughnessFactor = 1.0f;
		MaterialTextures sphereTextures{};
		sphereTextures.albedo = BDT_RESOURCE.LoadImage2D(SphereAlbedoPath);
		sphereTextures.normal = BDT_RESOURCE.LoadImage2D(SphereNormalPath);
		sphereTextures.ao = BDT_RESOURCE.LoadImage2D(SphereAOPath);
		sphereTextures.roughness = BDT_RESOURCE.LoadImage2D(SphereRoughnessPath);
		sphereTextures.height = BDT_RESOURCE.LoadImage2D(SphereHeightPath);
		sphereTextures.metallic = BDT_RESOURCE.LoadImage2D(SphereMetallicPath);
		m_sphereMaterial = BDT_RESOURCE.LoadMaterial("RTBruteForceWorld.Sphere", sphereDesc, sphereTextures);
	}

	void RTBruteForceWorld::Update(float deltaTime)
	{
		Super::Update(deltaTime);
	}

	void RTBruteForceWorld::CreateActors()
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

		// 4. 윗면이 Y=0인 발판형 직육면체
		auto* platform = AddPrimitiveActor("Platform", EPrimitiveType::Box, m_platformMaterial);
		auto* platformRenderer = platform->GetPrimitiveRenderer();
		platformRenderer->SetPosition(vec3(0.0f, -0.15f, 0.0f));
		platformRenderer->SetScale(vec3(5.0f, 0.3f, 3.0f));

		// 5. 발판 위에 구 3개를 중심 간격 1.5로 배치
		constexpr cstr sphereTags[] = { "Sphere", "Sphere2", "Sphere3" };
		for (uint32 i = 0; i < 3; ++i)
		{
			auto* sphere = AddPrimitiveActor(sphereTags[i], EPrimitiveType::Sphere, m_sphereMaterial);
			const float x = (static_cast<float>(i) - 1.0f) * 1.5f;
			sphere->GetPrimitiveRenderer()->SetPosition(vec3(x, 0.5f, 0.0f));
		}

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
