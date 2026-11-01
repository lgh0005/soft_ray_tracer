#pragma once

namespace bdt
{
	BDT_CLASS_PTR(GLMesh)
	BDT_CLASS_PTR(Mesh)

	class Mesh
	{
		BDT_DISABLE_COPY(Mesh)
		BDT_DISABLE_MOVE(Mesh)

	public:
		virtual ~Mesh();
		virtual const GLMesh* GetGLMesh() const = 0;
		virtual uint32 GetTriangleCount() const = 0;
		virtual Array<uint32, 3> GetTriangle(uint32 index) const = 0;
		bool IsValid() const;

	protected:
		Mesh();
	};
}

