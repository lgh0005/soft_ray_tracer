#include "pch.h"
#include "World.h"

#include "Actor.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "ModelActor.h"
#include "PrimitiveActor.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "ModelRenderer.h"

namespace bdt
{
    World::World(const String& tag) : m_tag(tag) { }

    World::~World() { Clear(); }

    void World::Init()
    {
        // 중복 초기화 방지
        if (m_mainCamera)
            return;

        // 리소스 비동기 로드 요청
        LoadResources();

        // 메인 카메라 생성
        m_mainCamera = std::make_unique<CameraActor>("MainCamera");

        // Actor 생성 및 배치
        CreateActors();
    }

    void World::Update(float deltaTime)
    {
        // 초기화되지 않았다면 갱신하지 않음
        if (!m_mainCamera)
            return;

        // 메인 카메라 갱신
        m_mainCamera->Update(deltaTime);

        // 일반 Actor 갱신
        for (const auto& actor : m_actors)
            actor->Update(deltaTime);
    }

    void World::Submit(RenderScene& scene) const
    {
        // 1. 이전 프레임 렌더링 데이터 초기화
        scene.Clear();

        // 2. CameraData 구성
        if (!m_mainCamera) return;
        CameraComponent* camera = m_mainCamera->GetCameraComponent();
        scene.SetCamera(camera->GetCameraData());

        // 3. Actor 순회
        for (const auto& actor : m_actors)
        {
            if (!actor) continue;

            switch (actor->GetActorType())
            {
                case EActorType::Light:
                {
                    const auto* lightActor = static_cast<const LightActor*>(actor);
                    LightComponent* light = lightActor->GetLightComponent();
                    scene.AddLight(light->GetLightData());
                    break;
                }

                case EActorType::Model:
                {
                    const auto* modelActor = static_cast<const ModelActor*>(actor);
                    ModelRenderer* renderer = modelActor->GetModelRenderer();
                    if (renderer) scene.AddObjects(renderer->GetRenderObjects());
                    break;
                }

                case EActorType::Primitive:
                {
                    // TODO:
                    // PrimitiveRenderer에 Mesh / Material
                    // 조회 API가 구현되면 RenderObject 제출
                    break;
                }

                default:
                    break;
            }
        }
    }

    void World::Clear()
    {
        m_actors.clear();

        m_staticModelActors.clear();
        m_skinnedModelActors.clear();
        m_staticMeshActors.clear();

        m_pointLightActors.clear();
        m_spotLightActors.clear();
        m_directionalLightActors.clear();

        m_mainCamera.reset();
    }

    const String& World::GetTag() const
    {
        return m_tag;
    }

    CameraActor* World::GetMainCamera() const
    {
        return m_mainCamera.get();
    }

    void World::Resize(uint32 width, uint32 height) const
    {
        if (!m_mainCamera) return;
        auto camera = m_mainCamera->GetCameraComponent();
        if (camera) camera->SetViewportSize(width, height);
    }

    void World::LoadResources()
    {
        // 파생 World에서 리소스 로드 요청
    }

    void World::CreateActors()
    {
        // 파생 World에서 Actor 생성 및 배치
    }

    void World::LoadEditorPannels()
    {
        // 파생 World에서 Editor 패널 추가 및 배치
    }
}
