#include "pch.h"
#include "WorldManager.h"
#include "World.h"

namespace bdt
{
    WorldManager::WorldManager() = default;
    WorldManager::~WorldManager() = default;

    void WorldManager::Update(float deltaTime)
    {
        // 예약된 World 전환 처리
        ApplyWorldChange();

        // 현재 World 갱신
        if (m_currentWorld)
            m_currentWorld->Update(deltaTime);
    }

    void WorldManager::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0) return;
        if (m_currentWorld) m_currentWorld->Resize(width, height);
    }

    void WorldManager::Submit()
    {
        if (m_currentWorld)
            m_currentWorld->Submit(m_renderScene);
    }

    void WorldManager::Shutdown()
    {
        // 전환 대기 상태 해제
        m_pendingWorld = nullptr;

        // 현재 World 정리
        if (m_currentWorld)
        {
            m_currentWorld->Clear();
            m_currentWorld = nullptr;
        }

        // 등록된 모든 World 파괴
        m_worlds.clear();
        m_renderScene.Clear();
    }

    void WorldManager::RegisterWorld(WorldUPtr world)
    {
        if (!world) return;

        // 중복 태그 등록 방지
        const String tag = world->GetTag();
        if (m_worlds.contains(tag)) return;

        m_worlds.emplace(tag, std::move(world));
    }

    void WorldManager::ChangeWorld(const String& tag)
    {
        World* nextWorld = GetWorld(tag);

        // 존재하지 않거나 이미 활성화된 World라면 무시
        if (!nextWorld)
            return;

        if (nextWorld == m_currentWorld)
        {
            // 기존 전환 요청도 취소
            m_pendingWorld = nullptr;
            return;
        }

        // 다음 Update에서 전환
        m_pendingWorld = nextWorld;
    }

    World* WorldManager::GetCurrentWorld() const
    {
        return m_currentWorld;
    }

    World* WorldManager::GetWorld(const String& tag) const
    {
        auto it = m_worlds.find(tag);
        if (it == m_worlds.end()) return nullptr;
        return it->second.get();
    }

    const RenderScene& WorldManager::GetRenderScene() const
    {
        return m_renderScene;
    }

    void WorldManager::ApplyWorldChange()
    {
        // 예약된 World가 없다면 전환하지 않음
        if (!m_pendingWorld)
            return;

        // 기존 World 정리
        if (m_currentWorld)
            m_currentWorld->Clear();

        // 새로운 World 활성화
        m_currentWorld = m_pendingWorld;
        m_pendingWorld = nullptr;

        // 새로운 World 초기화
        m_currentWorld->Init();
    }
}