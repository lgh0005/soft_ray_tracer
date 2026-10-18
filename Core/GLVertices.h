#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
    struct alignas(BDT_STD140_BASE_ALIGNMENT) GLSimpleVertex
    {
        vec3     position { 0.0f };
        vec4     color    { 1.0f };
    };

    struct alignas(BDT_STD140_BASE_ALIGNMENT) GLScreenVertex
    {
        vec3     position { 0.0f };
        vec2     texCoord { 0.0f };
    };

    struct alignas(BDT_STD140_BASE_ALIGNMENT) GLStaticVertex
    {
        vec3     position { 0.0f };
        vec3     normal   { 0.0f, 1.0f, 0.0f };
        vec2     texCoord { 0.0f };
        vec3     tangent  { 1.0f, 0.0f, 0.0f };
    };

    struct alignas(BDT_STD140_BASE_ALIGNMENT) GLSkinnedVertex
    {
        vec3            position { 0.0f };
        vec3            normal   { 0.0f, 1.0f, 0.0f };
        vec2            texCoord { 0.0f };
        vec3            tangent  { 1.0f, 0.0f, 0.0f };
        Array<int32, 4> boneIDs  { -1, -1, -1, -1 };
        Array<float, 4> weights  { 0.0f };
    };
}
