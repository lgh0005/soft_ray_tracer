#include "pch.h"
#include "GLProgramBuilder.h"

namespace bdt
{
    namespace
    {
        String GetShaderLog(uint32 shaderID)
        {
            GLint length = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
            if (length <= 1) return {};

            String log(static_cast<usize>(length), '\0');
            GLsizei written = 0;
            glGetShaderInfoLog(shaderID, length, &written, log.data());

            log.resize(static_cast<usize>(written));
            return log;
        }

        String GetProgramLog(uint32 programID)
        {
            GLint length = 0;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
            if (length <= 1) return {};

            String log(static_cast<usize>(length), '\0');
            GLsizei written = 0;
            glGetProgramInfoLog(programID, length, &written, log.data());

            log.resize(static_cast<usize>(written));
            return log;
        }
    }

    GLProgramBuilder::GLProgramBuilder()
    {
        m_programID = glCreateProgram();
    }

    GLProgramBuilder::~GLProgramBuilder()
    {
        ClearShaders();
        if (m_programID != 0)
        {
            glDeleteProgram(m_programID);
            m_programID = 0;
        }
    }

    bool GLProgramBuilder::AttachShader(GLenum type, const String& source, String* error)
    {
        if (m_programID == 0) return false;

        // 1. 셰이더 객체 생성
        uint32 shaderID = glCreateShader(type);
        if (shaderID == 0) return false;

        // 2. 소스 코드 등록
        const char* shaderSource = source.c_str();

        glShaderSource(shaderID, 1, &shaderSource, nullptr);

        // 3. 셰이더 컴파일
        glCompileShader(shaderID);

        GLint success = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE)
        {
            if (error) *error = GetShaderLog(shaderID);
            glDeleteShader(shaderID);
            return false;
        }

        // 4. 프로그램에 셰이더 연결
        glAttachShader(m_programID, shaderID);
        m_shaderIDs.push_back(shaderID);

        return true;
    }

    bool GLProgramBuilder::AttachShaderFile(const ShaderFile& file, String* error)
    {
        // 1. 셰이더 파일 읽기
        std::ifstream stream(file.path, std::ios::binary);

        if (!stream.is_open())
        {
            if (error) *error = "Failed to open shader file: " + file.path;
            return false;
        }

        std::ostringstream buffer;
        buffer << stream.rdbuf();

        if (stream.bad())
        {
            if (error) *error = "Failed to read shader file: " + file.path;
            return false;
        }

        // 2. GLSL 소스 컴파일
        if (!AttachShader(file.type, buffer.str(), error))
        {
            if (error) *error = file.path + "\n" + *error;
            return false;
        }

        return true;
    }

    uint32 GLProgramBuilder::Link(String* error)
    {
        if (m_programID == 0) return 0;
        if (m_shaderIDs.empty()) return 0;

        // 1. 프로그램 링크
        glLinkProgram(m_programID);

        GLint success = GL_FALSE;
        glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

        if (success != GL_TRUE)
        {
            if (error) *error = GetProgramLog(m_programID);
            return 0;
        }

        // 2. 링크에 사용된 셰이더 객체 정리
        ClearShaders();

        // 3. 프로그램 ID 소유권 이전
        uint32 programID = m_programID;
        m_programID = 0;

        return programID;
    }

    uint32 GLProgramBuilder::Build(const List<ShaderFile>& files, String* error)
    {
        if (error) error->clear();

        if (files.empty()) return 0;

        // 1. 전달된 셰이더 파일 컴파일
        for (const ShaderFile& file : files)
        {
            if (!AttachShaderFile(file, error))
                return 0;
        }

        // 2. 프로그램 링크
        return Link(error);
    }

    void GLProgramBuilder::ClearShaders()
    {
        for (uint32 shaderID : m_shaderIDs)
        {
            if (m_programID != 0) glDetachShader(m_programID, shaderID);
            glDeleteShader(shaderID);
        }

        m_shaderIDs.clear();
    }
}