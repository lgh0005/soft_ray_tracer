#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
    // [설명]
    class Profiler
    {
        BDT_DISABLE_COPY(Profiler)
        BDT_DISABLE_MOVE(Profiler)

    private:
        Profiler() = delete;

    public:
        static double _Internal_GetElapsedMilliseconds(SteadyTimePoint start, SteadyTimePoint end);
        static void _Internal_Report(StringView name, SteadyTimePoint start, SteadyTimePoint end, SourceLocation location);
    };

    // [설명]
    class ProfileScope
    {
        BDT_DISABLE_COPY(ProfileScope)
        BDT_DISABLE_MOVE(ProfileScope)

    public:
        explicit ProfileScope(StringView name, SourceLocation location = SourceLocation::current());
        ~ProfileScope();

    private:
        StringView m_name;
        SteadyTimePoint m_start;
        SourceLocation m_location;
    };
}

/*==========================================//
//              Profiler Macros             //
//==========================================*/
#define BDT_PROFILE_CONCAT_IMPL(x, y) x##y
#define BDT_PROFILE_CONCAT(x, y) BDT_PROFILE_CONCAT_IMPL(x, y)

#ifdef _DEBUG

// 스코프 실행 시간 측정
#define BDT_PROFILE_SCOPE(name) bdt::ProfileScope BDT_PROFILE_CONCAT(_bdt_profile_scope_, __COUNTER__)(name)

// 함수 실행 시간 측정
#define BDT_PROFILE_FUNCTION() BDT_PROFILE_SCOPE(__func__)

// 수동 시작 및 종료 시간 측정
#define BDT_PROFILE_START(tag) bdt::SteadyTimePoint BDT_PROFILE_CONCAT(_bdt_profile_start_, tag) = bdt::SteadyClock::now()

#define BDT_PROFILE_END(tag)                           \
    bdt::Profiler::_Internal_Report                    \
    (                                                  \
        #tag,                                          \
        BDT_PROFILE_CONCAT(_bdt_profile_start_, tag),  \
        bdt::SteadyClock::now(),                       \
        bdt::SourceLocation::current()                 \
    )

#else

#define BDT_PROFILE_SCOPE(name) ((void)0)
#define BDT_PROFILE_FUNCTION()  ((void)0)
#define BDT_PROFILE_START(tag)  ((void)0)
#define BDT_PROFILE_END(tag)    ((void)0)

#endif