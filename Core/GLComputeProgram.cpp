#include "pch.h"
#include "GLComputeProgram.h"
#include "GLProgramBuilder.h"

namespace bdt
{
    GLComputeProgram::GLComputeProgram() = default;
    GLComputeProgram::~GLComputeProgram() { Clear(); }

    GLComputeProgramUPtr GLComputeProgram::Create(const List<ShaderFile>& files, String* error)
    {
        GLComputeProgramUPtr program(new GLComputeProgram());
        if (!program->Init(files, error)) return nullptr;
        return program;
    }

    bool GLComputeProgram::Init(const List<ShaderFile>& files, String* error)
    {
        if (files.empty())
        {
            if (error) *error = "No compute shader files provided.";
            return false;
        }

        // Compute Shader 외 다른 단계의 셰이더는 허용하지 않음
        for (const ShaderFile& file : files)
        {
            if (file.type != GL_COMPUTE_SHADER)
            {
                if (error) *error = "Only compute shaders are allowed.";
                return false;
            }
        }

        GLProgramBuilder builder;
        m_programID = builder.Build(files, error);

        return IsValid();
    }

    void GLComputeProgram::Clear()
    {
        if (!IsValid()) return;
        glDeleteProgram(m_programID);
        m_programID = 0;
    }

    void GLComputeProgram::Bind() const
    {
        if (!IsValid()) return;
        glUseProgram(m_programID);
    }

    void GLComputeProgram::Unbind()
    {
        glUseProgram(0);
    }

    uint32 GLComputeProgram::GetProgramID() const
    {
        return m_programID;
    }

    bool GLComputeProgram::IsValid() const
    {
        return m_programID != 0;
    }
}