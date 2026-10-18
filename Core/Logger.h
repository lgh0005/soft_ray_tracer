#pragma once
#include "Defines.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace bdt
{
	class Logger
	{
		BDT_DISABLE_COPY(Logger)
		BDT_DISABLE_MOVE(Logger)
		using SPDLoggerPtr = std::shared_ptr<spdlog::logger>;

	private:
		Logger() = delete;
		
	public:
		static void _Internal_Init();
		static void _Internal_Clear();
		static void _Internal_Flush();

	public:
		template<typename... Args>
		static void _Internal_LogTrace(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogInfo(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogWarn(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogError(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogFatal(fmt::format_string<Args...> fmt, Args&&... args);
	
	private:
		static SPDLoggerPtr m_logger;
	};
}

#include "Logger.inl"

/*==========================================//
//                Logger Macros             //
//==========================================*/
#define BDT_LOG_INIT()       bdt::Logger::_Internal_Init()
#define BDT_LOG_CLEAR()      bdt::Logger::_Internal_Clear()
#define BDT_LOG_FLUSH()      bdt::Logger::_Internal_Flush()

#ifdef _DEBUG
#define BDT_LOG_TRACE(...)   bdt::Logger::_Internal_LogTrace(__VA_ARGS__)
#define BDT_LOG_INFO(...)    bdt::Logger::_Internal_LogInfo(__VA_ARGS__)
#define BDT_LOG_WARN(...)    bdt::Logger::_Internal_LogWarn(__VA_ARGS__)
#define BDT_LOG_ERROR(...)   bdt::Logger::_Internal_LogError(__VA_ARGS__)
#define BDT_LOG_FATAL(...)   bdt::Logger::_Internal_LogFatal(__VA_ARGS__)
#else
#define BDT_LOG_TRACE(...)   ((void)0)
#define BDT_LOG_INFO(...)    ((void)0)
#define BDT_LOG_WARN(...)    ((void)0)
#define BDT_LOG_ERROR(...)   ((void)0)
#define BDT_LOG_FATAL(...)   ((void)0)
#endif