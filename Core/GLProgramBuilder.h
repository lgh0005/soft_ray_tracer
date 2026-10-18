#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>
#include "ShaderFile.h"

namespace bdt
{
    class GLProgramBuilder
    {
        BDT_DISABLE_COPY(GLProgramBuilder)
        BDT_DISABLE_MOVE(GLProgramBuilder)

    public:
        GLProgramBuilder();
        ~GLProgramBuilder();

        bool AttachShader(GLenum type, const String& source, String* error = nullptr);
        bool AttachShaderFile(const ShaderFile& file, String* error = nullptr);

        uint32 Link(String* error = nullptr);
        uint32 Build(const List<ShaderFile>& files, String* error = nullptr);

    private:
        void ClearShaders();

    private:
        uint32 m_programID{ 0 };
        List<uint32> m_shaderIDs;
    };
}