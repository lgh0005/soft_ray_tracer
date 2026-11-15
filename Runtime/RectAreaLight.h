#pragma once
#include "LightComponent.h"

namespace bdt
{
    class RectAreaLight : public LightComponent
    {
        using Super = LightComponent;

    public:
        explicit RectAreaLight(Actor* owner);
        ~RectAreaLight() override;

        LightData GetLightData() const override;
        mat4 GetViewProjectionMatrix() const override;

        void SetSize(const vec2& size);
        const vec2& GetSize() const;

        void SetTwoSided(bool enabled);
        bool IsTwoSided() const;

    private:
        vec2 m_size{ 2.0f, 2.0f };
        bool m_twoSided{ false };
    };
}

