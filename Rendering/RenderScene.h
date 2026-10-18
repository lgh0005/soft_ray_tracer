#pragma once
#include "RenderObject.h"
#include "Uniforms.h"
#include "Instances.h"

namespace bdt
{
    class RenderScene
    {
    public:
        RenderScene();
        ~RenderScene();

    public:
        void Clear();

        void SetCamera(const CameraData& camera);
        void AddObject(const RenderObject& object);
        void AddObjects(const List<RenderObject>& objects);
        void AddLight(const LightData& light);

        const CameraData& GetCamera() const;
        const List<const RenderObject*>& GetObjects() const;
        const List<const List<RenderObject>*>& GetObjectGroups() const;
        const List<LightData>& GetLights() const;

    private:
        CameraData m_camera{};

        List<const RenderObject*> m_objects;
        List<const List<RenderObject>*> m_objectGroups;
        List<LightData> m_lights;
    };
}