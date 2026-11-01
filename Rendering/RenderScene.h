#pragma once
#include "RenderObject.h"
#include "Uniforms.h"
#include "Instances.h"

namespace bdt
{
    BDT_CLASS_PTR(ImageBasedLight)

    class RenderScene
    {
    public:
        RenderScene();
        ~RenderScene();

    public:
        void Clear();

        void SetCamera(const CameraData& camera);
        void SetImageBasedLight(ImageBasedLight* imageBasedLight);
        const CameraData& GetCamera() const;
        ImageBasedLight* GetImageBasedLight() const;

        void AddObject(const RenderObject& object);
        void AddObjects(const List<RenderObject>& objects);
        void AddLight(const LightData& lightData, bool castShadow, const mat4& vp);

        const List<const RenderObject*>& GetObjects() const;
        const List<const List<RenderObject>*>& GetObjectGroups() const;
        const List<LightData>& GetLights() const;
        const List<ShadowData>& GetShadows() const;

    private:
        CameraData m_camera{};
        ImageBasedLight* m_imageBasedLight{ nullptr };

        List<const RenderObject*> m_objects;
        List<const List<RenderObject>*> m_objectGroups;
        List<LightData> m_lights;
        List<ShadowData> m_shadows;
    };
}