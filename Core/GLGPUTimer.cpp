#include "pch.h"
#include "GLGPUTimer.h"
#include "Logger.h"

namespace bdt
{
    namespace
    {
        constexpr usize SlotCount = 4;

        struct GPUTimerData
        {
            Array<GLuint, SlotCount * 2> queries{};
            usize readSlot{ 0 };
            usize pendingCount{ 0 };
            uint64 activeToken{ 0 };
        };

        GLFWwindow* s_context{ nullptr };
        GLint s_counterBits{ 0 };
        uint64 s_nextToken{ 0 };
        Dictionary<String, GPUTimerData> s_timers;
    }

    bool GLGPUTimer::_Internal_Init()
    {
        GLFWwindow* context = glfwGetCurrentContext();
        if (!context) return false;
        if (s_context) return context == s_context;

        GLint bits = 0;
        glGetQueryiv(GL_TIMESTAMP, GL_QUERY_COUNTER_BITS, &bits);
        if (bits <= 0 || bits > 64) return false;
        s_context = context;
        s_counterBits = bits;
        return true;
    }

    bool GLGPUTimer::_Internal_Clear()
    {
        if (!s_context) return true;
        if (glfwGetCurrentContext() != s_context) return false;

        for (auto& [name, timer] : s_timers)
            glDeleteQueries(static_cast<GLsizei>(timer.queries.size()), timer.queries.data());
        s_timers.clear();
        s_context = nullptr;
        s_counterBits = 0;

        // 이전 스코프가 재초기화한 같은 이름의 측정을 종료하지 않도록 토큰은 유지한다.
        return true;
    }

    uint64 GLGPUTimer::_Internal_Start(StringView name)
    {
        if (!s_context || glfwGetCurrentContext() != s_context) return 0;

        auto [it, inserted] = s_timers.try_emplace(String(name));
        GPUTimerData& timer = it->second;
        if (inserted)
        {
            glCreateQueries(GL_TIMESTAMP, static_cast<GLsizei>(timer.queries.size()), timer.queries.data());
            for (GLuint query : timer.queries)
            {
                if (!query)
                {
                    glDeleteQueries(static_cast<GLsizei>(timer.queries.size()), timer.queries.data());
                    s_timers.erase(it);
                    return 0;
                }
            }
        }
        if (timer.activeToken || timer.pendingCount == SlotCount) return 0;
        const usize slot = (timer.readSlot + timer.pendingCount) % SlotCount;
        glQueryCounter(timer.queries[slot * 2], GL_TIMESTAMP);
        if (++s_nextToken == 0) ++s_nextToken;
        timer.activeToken = s_nextToken;
        return timer.activeToken;
    }

    bool GLGPUTimer::_Internal_End(StringView name, uint64 token)
    {
        if (!token || !s_context || glfwGetCurrentContext() != s_context) return false;
        auto it = s_timers.find(String(name));
        if (it == s_timers.end()) return false;
        GPUTimerData& timer = it->second;
        if (timer.activeToken != token) return false;
        const usize slot = (timer.readSlot + timer.pendingCount) % SlotCount;
        glQueryCounter(timer.queries[slot * 2 + 1], GL_TIMESTAMP);
        ++timer.pendingCount;
        timer.activeToken = 0;
        return true;
    }

    bool GLGPUTimer::_Internal_TryGetElapsedMilliseconds(StringView name, double& milliseconds)
    {
        if (!s_context || glfwGetCurrentContext() != s_context) return false;
        auto it = s_timers.find(String(name));
        if (it == s_timers.end()) return false;
        GPUTimerData& timer = it->second;
        if (timer.pendingCount == 0) return false;

        const GLuint startQuery = timer.queries[timer.readSlot * 2];
        const GLuint endQuery = timer.queries[timer.readSlot * 2 + 1];
        GLint startReady = GL_FALSE, endReady = GL_FALSE;
        glGetQueryObjectiv(startQuery, GL_QUERY_RESULT_AVAILABLE, &startReady);
        glGetQueryObjectiv(endQuery, GL_QUERY_RESULT_AVAILABLE, &endReady);
        if (!startReady || !endReady) return false;

        GLuint64 start = 0, end = 0;
        glGetQueryObjectui64v(startQuery, GL_QUERY_RESULT, &start);
        glGetQueryObjectui64v(endQuery, GL_QUERY_RESULT, &end);
        const uint64 mask = s_counterBits == 64 ? MaxValue<uint64> : (uint64(1) << s_counterBits) - 1;

        // 유효 비트 폭 안에서 발생한 타임스탬프의 한 번의 순환을 처리한다.
        milliseconds = static_cast<double>((end - start) & mask) / 1000000.0;
        timer.readSlot = (timer.readSlot + 1) % SlotCount;
        --timer.pendingCount;
        return true;
    }

    void GLGPUTimer::_Internal_Collect()
    {
        if (!s_context || glfwGetCurrentContext() != s_context) return;
        for (const auto& [name, timer] : s_timers)
        {
            double milliseconds = 0;
            while (_Internal_TryGetElapsedMilliseconds(name, milliseconds))
                BDT_LOG_INFO("[GPU Profile] {} : {:.3f} ms", name, milliseconds);
        }
    }

    GPUProfileScope::GPUProfileScope(StringView name)
        : m_name(name), m_token(GLGPUTimer::_Internal_Start(m_name))
    {}

    GPUProfileScope::~GPUProfileScope()
    {
        End();
    }

    void GPUProfileScope::End()
    {
        if (m_token && GLGPUTimer::_Internal_End(m_name, m_token)) m_token = 0;
    }
}
