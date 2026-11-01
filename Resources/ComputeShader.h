#pragma once
#include <Core/ShaderFile.h>

namespace bdt
{
	BDT_CLASS_PTR(GLComputeProgram)
	BDT_CLASS_PTR(ComputeShader)

	class ComputeShader
	{
		BDT_DISABLE_COPY(ComputeShader)
		BDT_DISABLE_MOVE(ComputeShader)

	public:
		~ComputeShader();
		static ComputeShaderUPtr Create
		(
			const List<ShaderFile>& files, 
			String* error = nullptr
		);
		void Bind() const;
		static void Unbind();
		bool IsValid() const;
		uint32 GetProgramID() const;
		void Dispatch(GLuint groupX, GLuint groupY = 1, GLuint groupZ = 1) const;

	private:
		ComputeShader();
		bool Init(const List<ShaderFile>& files, String* error);

		List<ShaderFile> m_files;
		GLComputeProgramUPtr m_program{ nullptr };
	};
}

