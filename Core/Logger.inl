#pragma once

namespace bdt
{
    template<typename... Args>
    inline void Logger::_Internal_LogTrace(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!m_logger) return;

        m_logger->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void Logger::_Internal_LogInfo(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!m_logger) return;

        m_logger->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void Logger::_Internal_LogWarn(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!m_logger) return;

        m_logger->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void Logger::_Internal_LogError(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!m_logger) return;

        m_logger->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void Logger::_Internal_LogFatal(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!m_logger) return;

        m_logger->critical(fmt, std::forward<Args>(args)...);
    }
}