#include "pch.h"
#include "Profiler.h"
#include "Logger.h"

namespace bdt
{
    /*==========================================//
    //                  Profiler               //
    //==========================================*/
    double Profiler::_Internal_GetElapsedMilliseconds(SteadyTimePoint start, SteadyTimePoint end)
    {
        MillisecondDuration elapsed = end - start;
        return elapsed.count();
    }

    void Profiler::_Internal_Report(StringView name, SteadyTimePoint start, SteadyTimePoint end, SourceLocation location)
    {
        double elapsed = _Internal_GetElapsedMilliseconds(start, end);

        BDT_LOG_INFO
        (
            "[Profile] {} : {:.3f} ms ({}:{})",
            name,
            elapsed,
            location.file_name(),
            location.line()
        );
    }

    /*==========================================//
    //                ProfileScope             //
    //==========================================*/
    ProfileScope::ProfileScope(StringView name, SourceLocation location)
        : m_name(name), m_start(SteadyClock::now()), m_location(location)
    {}

    ProfileScope::~ProfileScope()
    {
        Profiler::_Internal_Report(m_name, m_start, SteadyClock::now(), m_location);
    }
}