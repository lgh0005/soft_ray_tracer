#pragma once
#include <Rendering/IRenderBus.h>

namespace bdt
{
	BDT_CLASS_PTR(GLBuffer)
	BDT_CLASS_PTR(InstanceBuffer)
	BDT_CLASS_PTR(MyGBuffer)
	BDT_CLASS_PTR(MySimpleBus)

	class MySimpleBus : public IRenderBus
	{
		BDT_DISABLE_COPY(MySimpleBus)
		BDT_DISABLE_MOVE(MySimpleBus)

	public:
		virtual ~MySimpleBus() override;
		static MySimpleBusUPtr Create();
		bool BeginFrame(const RenderScene& scene, uint32 width, uint32 height);
		virtual bool Resize(uint32 width, uint32 height) override;
		void Shutdown();

	public:
		MyGBuffer* GetGBuffer() const;
		uint32 GetLightCount() const;

	private:
		MySimpleBus();
		bool Init();

		MyGBufferUPtr m_gBuffer{ nullptr };
		GLBufferUPtr m_cameraBuffer{ nullptr };
		InstanceBufferUPtr m_lightBuffer{ nullptr };
		uint32 m_lightCount{ 0 };
	};
}
