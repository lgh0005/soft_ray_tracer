#pragma 
#include <Resources/ResourceHandle.h>
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
	BDT_CLASS_PTR(StaticRenderQueue)
	BDT_CLASS_PTR(SkinnedRenderQueue)
	BDT_CLASS_PTR(MySimpleGeometryPass)

	class MySimpleGeometryPass : public IRenderPass
	{
		BDT_DISABLE_COPY(MySimpleGeometryPass)
		BDT_DISABLE_MOVE(MySimpleGeometryPass)

	public:
		virtual ~MySimpleGeometryPass() override;
		static MySimpleGeometryPassUPtr Create();
		virtual void Execute(IRenderBus& bus) override;
		virtual void Shutdown() override;

	private:
		void SubmitObject(const RenderObject& object);

	private:
		MySimpleGeometryPass();
		virtual bool Init() override;

		ResourceHandle<GraphicsShader> m_geometryShaderStatic;
		ResourceHandle<GraphicsShader> m_geometryShaderSkinned;
		StaticRenderQueueUPtr m_staticQueue{ nullptr };
		SkinnedRenderQueueUPtr m_skinnedQueue{ nullptr };
	};
}
