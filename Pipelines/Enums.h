#pragma once
#include <Core/Types.h>

namespace bdt
{
    enum class EGBufferTextureSlot : uint8
    {
        Position = 0,
        Normal,
        Albedo,
        Material,
        Emission,
        Depth,
        Count
    };
}
