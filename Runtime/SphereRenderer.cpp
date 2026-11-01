#include "pch.h"
#include "SphereRenderer.h"
#include <Resources/StaticMesh.h>
#include <glm/gtc/constants.hpp>

namespace bdt
{
	SphereRenderer::SphereRenderer(Actor* owner, const ResourceHandle<Material>& material)
		: Super(owner, material) { }
	SphereRenderer::~SphereRenderer() = default;
	EPrimitiveType SphereRenderer::GetPrimitiveType() const { return EPrimitiveType::Sphere; }

	StaticMeshPtr SphereRenderer::CreateMesh() const
	{
		constexpr uint32 slices = 32, stacks = 16;
		List<GLStaticVertex> vertices;
		List<uint32> indices;
		vertices.reserve((slices + 1) * (stacks + 1));
		indices.reserve(slices * (stacks - 1) * 6);

		for (uint32 latitude = 0; latitude <= stacks; ++latitude)
		{
			const float v = static_cast<float>(latitude) / stacks;
			const float phi = glm::pi<float>() * v;
			const float sinPhi = latitude == 0 || latitude == stacks ? 0.0f : glm::sin(phi);
			const float cosPhi = latitude == 0 ? 1.0f : latitude == stacks ? -1.0f : glm::cos(phi);
			for (uint32 longitude = 0; longitude <= slices; ++longitude)
			{
				const float u = static_cast<float>(longitude) / slices;
				const float theta = longitude == slices ? 0.0f : glm::two_pi<float>() * u;
				GLStaticVertex vertex{};
				vertex.normal = vec3(sinPhi * glm::cos(theta), cosPhi, sinPhi * glm::sin(theta));
				vertex.position = vertex.normal * 0.5f;
				vertex.tangent = vec3(-glm::sin(theta), 0.0f, glm::cos(theta));
				vertex.texCoord = vec2(u, 1.0f - v);
				vertices.push_back(vertex);
			}
		}

		for (uint32 latitude = 0; latitude < stacks; ++latitude)
		{
			for (uint32 longitude = 0; longitude < slices; ++longitude)
			{
				const uint32 a = latitude * (slices + 1) + longitude;
				const uint32 b = a + 1, c = a + slices + 1, d = c + 1;
				if (latitude > 0) indices.insert(indices.end(), { a, b, c });
				if (latitude + 1 < stacks) indices.insert(indices.end(), { b, d, c });
			}
		}

		return StaticMesh::Create(vertices, indices);
	}
}
