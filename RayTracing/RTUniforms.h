#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

namespace bdt
{
    struct alignas(BDT_STD140_BASE_ALIGNMENT) RTFrameData
    {
        uint32 rayCount{ 0 };
        uint32 instanceCount{ 0 };
        uint32 tlasDescriptorIndex{ MaxValue<uint32> };
        uint32 pad1{ 0 };
    };

    struct alignas(BDT_STD140_BASE_ALIGNMENT) RTRenderSettings
    {
        uint32 maxBounces{ 3 };
        uint32 maxAreaSampleCount{ 8 };
        uint32 traceResolutionDivisor{ 1 };
        uint32 denoiseEnabled{ 0 };
        uint32 temporal{ 1 };
        uint32 spatialIterations{ 1 };
        uint32 maxSampleCount{ 64 };
        uint32 aoEnabled{ 0 };
        uint32 visualize{ 0 };
        uint32 sampleCount{ 8 };
        float radius{ 1.0f };
        uint32 pad0{ 0 };
        uint32 environmentEnabled{ 0 };
        uint32 iblEnabled{ 0 };
        uint32 iblSampleCount{ 1 };
        uint32 iblSamplingMode{ 2 }; // 0: cosine reference, 1: environment reference, 2: MIS
        float iblIntensity{ 1.0f };
        float environmentRotation{ 0.0f }; // radians around world Y
        uint32 iblPad0{ 0 };
        uint32 iblPad1{ 0 };
    };
}
