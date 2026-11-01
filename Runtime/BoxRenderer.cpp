#include "pch.h"
#include "BoxRenderer.h"
#include <Resources/StaticMesh.h>

namespace bdt
{
	BoxRenderer::BoxRenderer(Actor* owner, const ResourceHandle<Material>& material)
		: Super(owner, material) { }
	BoxRenderer::~BoxRenderer() = default;
	EPrimitiveType BoxRenderer::GetPrimitiveType() const { return EPrimitiveType::Box; }

	StaticMeshPtr BoxRenderer::CreateMesh() const
	{
		// 각 면의 정점을 분리해 평면 법선과 독립된 0..1 UV를 갖는다.
		const Array<vec3, 6> normals
		{
			vec3(1, 0, 0), vec3(-1, 0, 0), vec3(0, 1, 0),
			vec3(0, -1, 0), vec3(0, 0, 1), vec3(0, 0, -1)
		};

		const Array<vec3, 6> tangents
		{
			vec3(0, 0, -1), vec3(0, 0, 1), vec3(1, 0, 0),
			vec3(1, 0, 0), vec3(1, 0, 0), vec3(-1, 0, 0)
		};

		const Array<vec2, 4> uvs{ vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1) };
		List<GLStaticVertex> vertices;
		List<uint32> indices;
		vertices.reserve(24);
		indices.reserve(36);

		for (uint32 face = 0; face < normals.size(); ++face)
		{
			const vec3 normal = normals[face], tangent = tangents[face];
			const vec3 bitangent = glm::cross(normal, tangent);
			const uint32 start = static_cast<uint32>(vertices.size());
			for (const vec2& uv : uvs)
			{
				GLStaticVertex vertex{};
				vertex.position = normal * 0.5f + tangent * (uv.x - 0.5f) + bitangent * (uv.y - 0.5f);
				vertex.normal = normal;
				vertex.tangent = tangent;
				vertex.texCoord = uv;
				vertices.push_back(vertex);
			}
			indices.insert(indices.end(), { start, start + 1, start + 2, start, start + 2, start + 3 });
		}

		return StaticMesh::Create(vertices, indices);
	}
}
