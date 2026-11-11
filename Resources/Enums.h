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
        Metallic = 6, // Keep existing texture units stable; unit 5 is reserved.
        Specular,
        Count
    };
}
