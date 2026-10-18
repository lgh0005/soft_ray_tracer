#include "pch.h"
#include "GLGraphicsProgram.h"
#include "GLProgramBuilder.h"

namespace bdt
{
    GLGraphicsProgram::GLGraphicsProgram() = default;
    GLGraphicsProgram::~GLGraphicsProgram() { Clear(); }

    GLGraphicsProgramUPtr GLGraphicsProgram::Create
    (
        const List<ShaderFile>& files, 
        String* error
    )
    {
        GLGraphicsProgramUPtr program(new GLGraphicsProgram());
        if (!program->Init(files, error)) return nullptr;
        return program;
    }

    bool GLGraphicsProgram::Init
    (
        const List<ShaderFile>& files, 
        String* error
    )
    {
        // Compute Shader 혼합 방지
        for (const ShaderFile& file : files)
        {
            if (file.type == GL_COMPUTE_SHADER)
            {
                if (error) *error = "Compute shader cannot be attached to a graphics program.";
                return false;
            }
        }

        GLProgramBuilder builder;
        m_programID = builder.Build(files, error);

        return IsValid();
    }

    void GLGraphicsProgram::Clear()
    {
        if (!IsValid()) return;
        glDeleteProgram(m_programID);
        m_programID = 0;
    }

    void GLGraphicsProgram::Bind() const
    {
        if (!IsValid()) return;
        glUseProgram(m_programID);
    }

    void GLGraphicsProgram::Unbind()
    {
        glUseProgram(0);
    }

    uint32 GLGraphicsProgram::GetProgramID() const
    {
        return m_programID;
    }

    bool GLGraphicsProgram::IsValid() const
    {
        return m_programID != 0;
    }
}