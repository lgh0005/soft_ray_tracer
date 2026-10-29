#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
    class GLGPUTimer
    {
        BDT_DISABLE_COPY(GLGPUTimer)
        BDT_DISABLE_MOVE(GLGPUTimer)

    private:
        GLGPUTimer() = delete;

    public:
        static bool _Internal_Init();
        static bool _Internal_Clear();
        static uint64 _Internal_Start(StringView name);
        static bool _Internal_End(StringView name, uint64 token);
        static void _Internal_Collect();
        static bool _Internal_TryGetElapsedMilliseconds(StringView name, double& milliseconds);
    };

    class GPUProfileScope
    {
        BDT_DISABLE_COPY(GPUProfileScope)
        BDT_DISABLE_MOVE(GPUProfileScope)

    public:
        explicit GPUProfileScope(StringView name);
        ~GPUProfileScope();
        void End();

    private:
        String m_name;
        uint64 m_token{ 0 };
    };
}

/*==========================================//
//             GPU Profiler Macros          //
//==========================================*/
#define BDT_GPU_PROFILE_CONCAT_IMPL(x, y) x##y
#define BDT_GPU_PROFILE_CONCAT(x, y) BDT_GPU_PROFILE_CONCAT_IMPL(x, y)

// Logger와 같이 초기화/정리는 모든 구성에서 명시적으로 호출할 수 있다.
#define BDT_GPU_PROFILE_INIT()  bdt::GLGPUTimer::_Internal_Init()
#define BDT_GPU_PROFILE_CLEAR() bdt::GLGPUTimer::_Internal_Clear()

#ifdef _DEBUG

// 이름별 미수집 결과는 최대 4개다. 다른 이름의 중첩 측정을 지원한다.
// 같은 이름이 이미 측정 중이거나 큐가 가득 차면 해당 측정을 건너뛴다.
#define BDT_GPU_PROFILE_SCOPE(name) bdt::GPUProfileScope BDT_GPU_PROFILE_CONCAT(_bdt_gpu_profile_scope_, __COUNTER__)(name)
#define BDT_GPU_PROFILE_FUNCTION() BDT_GPU_PROFILE_SCOPE(__func__)

// START/END는 같은 C++ 스코프에서 같은 식별자 tag로 짝을 이룬다.
// 조기 반환 시에도 소멸자가 측정을 종료한다.
#define BDT_GPU_PROFILE_START(tag) bdt::GPUProfileScope BDT_GPU_PROFILE_CONCAT(_bdt_gpu_profile_start_, tag)(#tag)
#define BDT_GPU_PROFILE_END(tag) BDT_GPU_PROFILE_CONCAT(_bdt_gpu_profile_start_, tag).End()
#define BDT_GPU_PROFILE_COLLECT() bdt::GLGPUTimer::_Internal_Collect()
#define BDT_GPU_PROFILE_TRY_GET(name, milliseconds) bdt::GLGPUTimer::_Internal_TryGetElapsedMilliseconds(name, milliseconds)

#else

#define BDT_GPU_PROFILE_SCOPE(name) ((void)0)
#define BDT_GPU_PROFILE_FUNCTION() ((void)0)
#define BDT_GPU_PROFILE_START(tag) ((void)0)
#define BDT_GPU_PROFILE_END(tag) ((void)0)
#define BDT_GPU_PROFILE_COLLECT() ((void)0)
#define BDT_GPU_PROFILE_TRY_GET(name, milliseconds) (false)

#endif
