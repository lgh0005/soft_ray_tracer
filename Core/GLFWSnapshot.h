#pragma once
#include "Defines.h"
#include "Types.h"

struct GLFWwindow;

namespace bdt
{
    class GLFWSnapshot
    {
        BDT_DISABLE_COPY(GLFWSnapshot)
        BDT_DISABLE_MOVE(GLFWSnapshot)

    public:
        GLFWSnapshot() = delete;

        static bool Save
        (
            GLFWwindow* window, 
            const String& fileName, 
            String* error = nullptr
        );

    private:
        static bool Fail(String* error, cstr message);
        static void Put
        (
            Array<uint8, 54>& header, 
            usize offset, 
            uint32 value, 
            usize bytes
        );
    };
}

/*==========================================//
//              Snapshot Macros             //
//==========================================*/
#define BDT_SNAPSHOT_SAVE(...) bdt::GLFWSnapshot::Save(__VA_ARGS__)
