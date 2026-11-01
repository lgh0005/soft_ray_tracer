#include "pch.h"
#include "ComputeShader.h"
#include <Core/GLComputeProgram.h>

namespace bdt
{
    ComputeShader::ComputeShader() = default;
    ComputeShader::~ComputeShader() = default;

    ComputeShaderUPtr ComputeShader::Create(const List<ShaderFile>& files, String* error)
    {
        ComputeShaderUPtr shader(new ComputeShader());
        if (!shader->Init(files, error)) return nullptr;
        return shader;
    }

    bool ComputeShader::Init(const List<ShaderFile>& files, String* error)
    {
        GLComputeProgramUPtr program = GLComputeProgram::Create(files, error);
        if (!program) return false;

        m_files = files;
        m_program = std::move(program);
        return true;
    }

    void ComputeShader::Bind() const
    {
        if (!IsValid()) return;
        m_program->Bind();
    }

    void ComputeShader::Unbind()
    {
        GLComputeProgram::Unbind();
    }

    bool ComputeShader::IsValid() const
    {
        return m_program && m_program->IsValid();
    }

    uint32 ComputeShader::GetProgramID() const
    {
        return m_program ? m_program->GetProgramID() : 0;
    }

    void ComputeShader::Dispatch(GLuint groupX, GLuint groupY, GLuint groupZ) const
    {
        if (!IsValid()) return;
        m_program->Dispatch(groupX, groupY, groupZ);
    }

}
