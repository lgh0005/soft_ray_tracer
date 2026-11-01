#pragma once
#include "RTGeometry.h"

namespace bdt
{
	BDT_CLASS_PTR(Mesh)
	BDT_CLASS_PTR(RTGeometryStorage)
	BDT_CLASS_PTR(RTInstanceStorage)

	class InstanceManager
	{
		BDT_DECLARE_SINGLE(InstanceManager)

	public:
		bool Init();
		void Shutdown();
		bool Prepare(const RenderScene& scene);

	/*==================================//
	//    Geometries for ray tracing    //
	//==================================*/
	public:
		void ClearGeometry();
		bool UploadGeometry();
		uint32 GetGeometryCount() const;

		bool RegisterMesh(const MeshPtr& mesh, bool isSkinned, uint32& idx);
		void BindStaticVertexBuffer() const;
		void BindSkinnedVertexBuffer() const;
		void BindIndexBuffer() const;
		void BindDescriptorBuffer() const;

	/*==================================//
	//     Instances for ray tracing    //
	//==================================*/
	public:
		void ClearInstances();
		bool UploadInstances();
		uint32 GetInstanceCount() const;
		uint32 GetBoneMatrixCount() const;
		uint32 GetSkinnedVertexCount() const;
		uint32 GetMaxSkinnedVertexCount() const;

		bool AddInstance(const GPUInstance& instance, uint32& idx);
		bool AppendBoneMatrices(const List<mat4>& matrices, uint32& offset);
		void BindInstanceBuffer() const;
		void BindBoneBuffer() const;
		void BindSkinnedOutputBuffer() const;

		bool AllocateSkinnedVertices(uint32 vertexCount, uint32& vertexOffset);

	private:

		// 개별 RenderObject를 GPUInstance로 변환
		bool AppendRenderObject(const RenderObject& object);

		// 현재 프레임의 Geometry / Instance GPU 업로드
		bool Upload();

		RTGeometryStorageUPtr m_geometryStorage;
		RTInstanceStorageUPtr m_instanceStorage;
	};
}

