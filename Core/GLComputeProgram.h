#pragma once
#include "Defines.h"
#include "Types.h"
#include "ShaderFile.h"

namespace bdt
{
    BDT_CLASS_PTR(GLComputeProgram)

    class GLComputeProgram
    {
        BDT_DISABLE_COPY(GLComputeProgram)
        BDT_DISABLE_MOVE(GLComputeProgram)

    public:
        ~GLComputeProgram();
        static GLComputeProgramUPtr Create
        (
            const List<ShaderFile>& files, 
            String* error = nullptr
        );

        void Clear();
        void Bind() const;
        static void Unbind();

    public:
        uint32 GetProgramID() const;
        void Dispatch(GLuint groupX, GLuint groupY = 1, GLuint groupZ = 1) const;
        bool IsValid() const;

    private:
        GLComputeProgram();
        bool Init(const List<ShaderFile>& files, String* error);

        uint32 m_programID{ 0 };
    };
}