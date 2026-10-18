#pragma once
#include <Core/Types.h>
#include "ResourceHandle.h"
#include "Image2D.h"

namespace bdt
{
    struct MaterialTexturePaths
    {
        String albedo;
        String normal;
        String ao;
        String roughness;
        String emission;
        String height;
    };

    struct MaterialTextures
    {
        ResourceHandle<Image2D> albedo;
        ResourceHandle<Image2D> normal;
        ResourceHandle<Image2D> ao;
        ResourceHandle<Image2D> roughness;
        ResourceHandle<Image2D> emission;
        ResourceHandle<Image2D> height;
    };

    struct MaterialDesc
    {
        vec4 albedoFactor      { 1.0f };
        vec3 emissiveFactor    { 0.0f };
        float metallicFactor   { 0.0f };
        float roughnessFactor  { 1.0f };
        float emissionStrength { 1.0f };
        float heightScale      { 0.0f };
    };
}