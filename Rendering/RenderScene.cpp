#include "pch.h"
#include "RenderScene.h"
#include "Enums.h"
#include "ImageBasedLight.h"

namespace bdt
{
    RenderScene::RenderScene() = default;
    RenderScene::~RenderScene() = default;

    void RenderScene::Clear()
    {
        m_camera = CameraData{};
        m_imageBasedLight = nullptr;

        m_objects.clear();
        m_objectGroups.clear();
        m_lights.clear();
        m_shadows.clear();
    }

    void RenderScene::SetCamera(const CameraData& camera)
    {
        m_camera = camera;
    }

    void RenderScene::SetImageBasedLight(ImageBasedLight* imageBasedLight)
    {
        m_imageBasedLight = imageBasedLight;
    }

    ImageBasedLight* RenderScene::GetImageBasedLight() const
    {
        return m_imageBasedLight;
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

    void RenderScene::AddLight(const LightData& lightData, bool castShadow, const mat4& vp)
    {
        LightData light = lightData;
        light.shadowIndex = -1;
        const uint32 lightIndex = static_cast<uint32>(m_lights.size());
        
        if (castShadow &&
            (light.type == static_cast<uint32>(ELightType::Directional) ||
             light.type == static_cast<uint32>(ELightType::Spot)))
        {
            ShadowData shadow{};
            light.shadowIndex = -1;
            shadow.lightViewProjection = vp;
            shadow.lightIndex = lightIndex;
            shadow.shadowMapIndex = static_cast<uint32>(m_shadows.size());
            light.shadowIndex = static_cast<int32>(m_shadows.size());
            m_shadows.push_back(shadow);
        }

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

    const List<ShadowData>& RenderScene::GetShadows() const
    {
        return m_shadows;
    }
}
