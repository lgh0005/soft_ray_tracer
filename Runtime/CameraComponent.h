#pragma once
#include "RootComponent.h"
#include <Rendering/RenderViewport.h>

namespace bdt
{
	class CameraComponent : public RootComponent
	{
		using Super = RootComponent;

	public:
		explicit CameraComponent(Actor* owner);
		virtual ~CameraComponent() override;

    public:
        float GetFOV() const;
        float GetNearPlane() const;
        float GetFarPlane() const;
        const RenderViewport& GetViewport() const;

        void SetFOV(float fov);
        void SetNearPlane(float nearPlane);
        void SetFarPlane(float farPlane);
        void SetViewportSize(uint32 width, uint32 height);

        mat4 GetViewMatrix() const;
        mat4 GetProjectionMatrix() const;
        CameraData GetCameraData() const;

    private:
        float m_fov{ 60.0f };
        float m_nearPlane{ 0.1f };
        float m_farPlane{ 1000.0f };
        RenderViewport m_viewport;
	};
}
