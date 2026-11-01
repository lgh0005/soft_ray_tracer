#include "pch.h"
#include "GraphicsShader.h"
#include <Core/GLGraphicsProgram.h>

namespace bdt
{
    GraphicsShader::GraphicsShader() = default;
    GraphicsShader::~GraphicsShader() = default;

    GraphicsShaderUPtr GraphicsShader::Create(const List<ShaderFile>& files, String* error)
    {
        GraphicsShaderUPtr shader(new GraphicsShader());
        if (!shader->Init(files, error)) return nullptr;
        return shader;
    }

    bool GraphicsShader::Init(const List<ShaderFile>& files, String* error)
    {
        GLGraphicsProgramUPtr program = GLGraphicsProgram::Create(files, error);
        if (!program) return false;

        m_files = files;
        m_program = std::move(program);
        return true;
    }

    void GraphicsShader::Bind() const
    {
        if (!IsValid()) return;
        m_program->Bind();
    }

    void GraphicsShader::Unbind()
    {
        GLGraphicsProgram::Unbind();
    }

    bool GraphicsShader::IsValid() const
    {
        return m_program && m_program->IsValid();
    }

    uint32 GraphicsShader::GetProgramID() const
    {
        return m_program ? m_program->GetProgramID() : 0;
    }
}
