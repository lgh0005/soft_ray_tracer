#include "pch.h"
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace bdt
{
    Logger::SPDLoggerPtr Logger::m_logger{ nullptr };

    void Logger::_Internal_Init()
    {
        if (m_logger) return;

        // 콘솔 로거 생성
        m_logger = spdlog::stdout_color_mt("BDT");

        // 로그 출력 설정
        m_logger->set_pattern("%^[%T] %n: %v%$");
        m_logger->set_level(spdlog::level::trace);

        _Internal_LogInfo("Logger initialized.");
    }

    void Logger::_Internal_Clear()
    {
        if (!m_logger) return;

        _Internal_LogInfo("Logger shutdown.");

        m_logger->flush();
        spdlog::drop("BDT");
        m_logger.reset();
    }

    void Logger::_Internal_Flush()
    {
        if (!m_logger) return;
        m_logger->flush();
    }
}