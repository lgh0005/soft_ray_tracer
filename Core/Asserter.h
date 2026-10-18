#pragma once
#include "Defines.h"
#include "Types.h"

/*==========================================//
//             Platform Breakpoint          //
//==========================================*/

#if defined(_MSC_VER)
#include <intrin.h>
#define BDT_PLATFORM_BREAK() __debugbreak()

#elif defined(__clang__) || defined(__GNUC__)
#define BDT_PLATFORM_BREAK() __builtin_trap()

#else
#include <cstdlib>
#define BDT_PLATFORM_BREAK() std::abort()
#endif

namespace bdt
{
    class Asserter
    {
        BDT_DISABLE_COPY(Asserter)
            BDT_DISABLE_MOVE(Asserter)

    private:
        Asserter() = delete;

    public:
        static void _Internal_ReportFailure(const char* condition, const char* message, const char* file, int32 line);
    };
}

/*==========================================//
//              Assertion Macros            //
//==========================================*/

// 컴파일 타임 검증
#define BDT_STATIC_ASSERT(condition, message) static_assert(condition, message)

// 런타임 검증
#ifdef _DEBUG

#define BDT_ASSERT(condition, message)                                        \
    do                                                                        \
    {                                                                         \
        if (!(condition))                                                     \
        {                                                                     \
            bdt::Asserter::_Internal_ReportFailure                            \
            (                                                                 \
                #condition, message, __FILE__, __LINE__                       \
            );                                                                \
            BDT_PLATFORM_BREAK();                                             \
        }                                                                     \
    } while (0)

#define BDT_ASSERT_BASE(condition) BDT_ASSERT(condition, "No Message")

#else

#define BDT_ASSERT(condition, message) ((void)0)
#define BDT_ASSERT_BASE(condition)     ((void)0)

#endif