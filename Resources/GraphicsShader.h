#pragma once
#include <Core/ShaderFile.h>

namespace bdt
{
	BDT_CLASS_PTR(GLGraphicsProgram)
	BDT_CLASS_PTR(GraphicsShader)

	class GraphicsShader
	{
		BDT_DISABLE_COPY(GraphicsShader)
		BDT_DISABLE_MOVE(GraphicsShader)

	public:
		~GraphicsShader();
		static GraphicsShaderUPtr Create
		(
			const List<ShaderFile>& files, 
			String* error = nullptr
		);
		void Bind() const;
		static void Unbind();
		bool IsValid() const;
		uint32 GetProgramID() const;

	private:
		GraphicsShader();
		bool Init(const List<ShaderFile>& files, String* error);

		List<ShaderFile> m_files;
		GLGraphicsProgramUPtr m_program{ nullptr };
	};
}
