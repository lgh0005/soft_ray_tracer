#pragma once
#include "Types.h"

namespace bdt
{
    enum class EInputState : uint8
    {
        None,
        Down,
        Press,
        Up
    };

    enum class EMouseButton : uint8
    {
        Left,
        Right,
        Middle,
        Count
    };
}