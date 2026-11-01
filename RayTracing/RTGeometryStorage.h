#pragma once
#include "RTGeometry.h"

namespace bdt
{
	BDT_CLASS_PTR(Mesh)
	BDT_CLASS_PTR(GLStorageBuffer)
	BDT_CLASS_PTR(RTGeometryStorage)

	class RTGeometryStorage
	{
		BDT_DISABLE_COPY(RTGeometryStorage)
		BDT_DISABLE_MOVE(RTGeometryStorage)

	public:
		~RTGeometryStorage();
		static RTGeometryStorageUPtr Create();
		bool Upload();
		void Clear();
		void Shutdown();

		bool RegisterMesh(const MeshPtr& mesh, bool isSkinned, uint32& idx);
		void BindStaticVertexBuffer() const;
		void BindSkinnedVertexBuffer() const;
		void BindIndexBuffer() const;
		void BindDescriptorBuffer() const;

		GLStorageBuffer* GetVertexBuffer() const;
		GLStorageBuffer* GetSkinnedVertexBuffer() const;
		GLStorageBuffer* GetIndexBuffer() const;
		GLStorageBuffer* GetDescriptorBuffer() const;
		uint32 GetGeometryCount() const;

	private:
		bool RegisterStaticMesh(const MeshPtr& mesh, uint32& idx);
		bool RegisterSkinnedMesh(const MeshPtr& mesh, uint32& idx);

		bool UploadStaticVertexBuffer();
		bool UploadSkinnedVertexBuffer();
		bool UploadIndexBuffer();
		bool UploadDescriptorBuffer();

	private:
		RTGeometryStorage();
		bool Init();

		bool m_isStorageDirty{ false };

		// Mesh별 캐시 식별
		List<MeshPtr> m_meshes;

		// CPU Geometry Cache
		List<uint32> m_indices;
		List<GPUStaticVertex> m_vertices;
		List<GPUSkinnedVertex> m_skinnedVertices;
		List<GPUMeshDescriptor> m_descriptors;

		// GPU Geometry Storage
		GLStorageBufferUPtr m_staticVertexBuffer;
		GLStorageBufferUPtr m_skinnedVertexBuffer;
		GLStorageBufferUPtr m_indexBuffer;
		GLStorageBufferUPtr m_descriptorBuffer;
	};
}

