#pragma once
#include <Core/Types.h>

namespace bdt
{
    enum class EMaterialTextureSlot : uint32
    {
        Albedo,
        Normal,
        AO,
        Roughness,
        Emission,
        Height,
        Count
    };
}