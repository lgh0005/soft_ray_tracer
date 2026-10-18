#include "pch.h"
#include "RenderScene.h"

namespace bdt
{
    RenderScene::RenderScene() = default;
    RenderScene::~RenderScene() = default;

    void RenderScene::Clear()
    {
        m_camera = CameraData{};
        m_objects.clear();
        m_objectGroups.clear();
        m_lights.clear();
    }

    void RenderScene::SetCamera(const CameraData& camera)
    {
        m_camera = camera;
    }

    void RenderScene::AddObject(const RenderObject& object)
    {
        m_objects.push_back(&object);
    }

    void RenderScene::AddObjects(const List<RenderObject>& objects)
    {
        if (objects.empty()) return;
        m_objectGroups.push_back(&objects);
    }

    void RenderScene::AddLight(const LightData& light)
    {
        m_lights.push_back(light);
    }

    const CameraData& RenderScene::GetCamera() const
    {
        return m_camera;
    }

    const List<const RenderObject*>& RenderScene::GetObjects() const
    {
        return m_objects;
    }

    const List<const List<RenderObject>*>& RenderScene::GetObjectGroups() const
    {
        return m_objectGroups;
    }

    const List<LightData>& RenderScene::GetLights() const
    {
        return m_lights;
    }
}
