#include "pch.h"
#include "TimeManager.h"

namespace bdt
{
    TimeManager::TimeManager() = default;
    TimeManager::~TimeManager() = default;

    bool TimeManager::Init()
    {
        // GLFWManager 초기화 이후 호출
        m_lastTime = glfwGetTime();
        m_currentTime = m_lastTime;
        m_deltaTime = 0.0f;

        m_fpsLastTime = m_currentTime;
        m_frameCount = 0;
        m_fps = 0.0f;

        return true;
    }

    void TimeManager::Update()
    {
        m_currentTime = glfwGetTime();

        // 실제 프레임 경과 시간
        double frameTime = m_currentTime - m_lastTime;
        m_lastTime = m_currentTime;

        // 비정상적인 시간 차이 방지
        if (frameTime < 0.0) frameTime = 0.0;

        m_deltaTime = static_cast<float>(frameTime);
        CalculateFPS();
    }

    void TimeManager::Shutdown()
    {
        m_lastTime = 0.0;
        m_currentTime = 0.0;
        m_deltaTime = 0.0f;
        m_fpsLastTime = 0.0;
        m_frameCount = 0;
        m_fps = 0.0f;
    }

    void TimeManager::CalculateFPS()
    {
        m_frameCount++;

        double elapsed = m_currentTime - m_fpsLastTime;

        if (elapsed >= 1.0)
        {
            m_fps = static_cast<float>(m_frameCount / elapsed);
            m_frameCount = 0;
            m_fpsLastTime = m_currentTime;
        }
    }
}