#pragma once
#include "Types.h"
#include <glad/gl.h>

namespace bdt
{
    struct ShaderFile
    {
        GLenum type;
        String path;
    };
}