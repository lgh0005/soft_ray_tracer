#pragma once
#include "IInputDevice.h"
#include "Defines.h"
#include "Types.h"
#include "Enums.h"

namespace bdt
{
    class Mouse : public IInputDevice
    {
        BDT_DISABLE_COPY(Mouse)
        BDT_DISABLE_MOVE(Mouse)
        using Super = IInputDevice;

    public:
        explicit Mouse(GLFWwindow* window);
        virtual ~Mouse() override;

    public:
        void Update() override;
        void Clear() override;

    public:
        bool GetButtonDown(EMouseButton button) const;
        bool GetButtonPress(EMouseButton button) const;
        bool GetButtonUp(EMouseButton button) const;
        bool GetButtonHold(EMouseButton button) const;
        EInputState GetButtonState(EMouseButton button) const;

    public:
        void HandleScroll(float delta);
        const vec2& GetPosition() const { return m_position; }
        const vec2& GetDelta() const { return m_delta; }
        float GetWheelDelta() const { return m_wheelDelta; }

    private:
        Array<EInputState, static_cast<usize>(EMouseButton::Count)> m_states{};

        vec2 m_position{ 0.0f };
        vec2 m_delta{ 0.0f };

        float m_wheelDelta{ 0.0f };
        float m_pendingWheelDelta{ 0.0f };

        bool m_hasPreviousPosition{ false };
    };
}