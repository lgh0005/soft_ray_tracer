#pragma once
#include <Rendering/RenderScene.h>

namespace bdt
{
    BDT_CLASS_PTR(World)

    class WorldManager
    {
        BDT_DECLARE_SINGLE(WorldManager)

    public:
        void Update(float deltaTime);
        void Resize(uint32 width, uint32 height);
        void Submit();
        void Shutdown();

        void RegisterWorld(WorldUPtr world);
        void ChangeWorld(const String& tag);

        World* GetCurrentWorld() const;
        World* GetWorld(const String& tag) const;
        const RenderScene& GetRenderScene() const;

    private:
        void ApplyWorldChange();

    private:
        Dictionary<String, WorldUPtr> m_worlds;
        World* m_currentWorld{ nullptr };
        World* m_pendingWorld{ nullptr };
        RenderScene m_renderScene;
    };
}