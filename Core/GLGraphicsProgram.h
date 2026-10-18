#pragma once
#include "Defines.h"
#include "Types.h"
#include "ShaderFile.h"

namespace bdt
{
    BDT_CLASS_PTR(GLGraphicsProgram)

    class GLGraphicsProgram
    {
        BDT_DISABLE_COPY(GLGraphicsProgram)
        BDT_DISABLE_MOVE(GLGraphicsProgram)

    public:
        ~GLGraphicsProgram();

        static GLGraphicsProgramUPtr Create
        (
            const List<ShaderFile>& files, 
            String* error = nullptr
        );

        void Clear();
        void Bind() const;
        static void Unbind();

    public:
        uint32 GetProgramID() const;
        bool IsValid() const;

    private:
        GLGraphicsProgram();
        bool Init(const List<ShaderFile>& files, String* error);

        uint32 m_programID{ 0 };
    };
}