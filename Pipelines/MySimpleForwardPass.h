#pragma once
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(MySimpleForwardPass)
	// 공유 큐의 불투명 씬 전체를 HDR로 그리는 대체 경로다.
	class MySimpleForwardPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleForwardPass)
		BDT_DISABLE_MOVE(MySimpleForwardPass)

	public:
		MySimpleForwardPass() = default;
		~MySimpleForwardPass() override;
		static MySimpleForwardPassUPtr Create();
		bool Init() override;
		void Execute(IRenderBus& bus) override;
		void Shutdown() override;

	private:
		ResourceHandle<GraphicsShader> m_staticShader;
		ResourceHandle<GraphicsShader> m_skinnedShader;
		bool m_initialized{ false };
	};
}
