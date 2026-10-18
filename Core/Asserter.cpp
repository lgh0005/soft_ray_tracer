#include "pch.h"
#include "Asserter.h"
#include "Logger.h"
#include "Types.h"

namespace bdt
{
    void Asserter::_Internal_ReportFailure(cstr condition, cstr message, cstr file, int32 line)
    {
        BDT_LOG_FATAL("Assertion Failed!");
        BDT_LOG_FATAL("[Condition] : {}", condition);
        BDT_LOG_FATAL("[Message]   : {}", message);
        BDT_LOG_FATAL("[Location]  : {}({})", file, line);
        BDT_LOG_FLUSH();
    }
}