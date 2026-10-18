#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
    class TimeManager
    {
        BDT_DECLARE_SINGLE(TimeManager)

    public:
        bool Init();
        void Update();
        void Shutdown();

    public:
        float GetDeltaTime() const { return m_deltaTime; }
        float GetFPS() const { return m_fps; }
        double GetTime() const { return m_currentTime; }

    private:
        void CalculateFPS();

    private:
        double m_lastTime{ 0.0 };
        double m_currentTime{ 0.0 };
        float m_deltaTime{ 0.0f };

        double m_fpsLastTime{ 0.0 };
        int32 m_frameCount{ 0 };
        float m_fps{ 0.0f };
    };
}