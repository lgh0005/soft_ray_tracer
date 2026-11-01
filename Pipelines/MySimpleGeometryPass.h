#pragma 
#include <Rendering/IRenderPass.h>

namespace bdt
{
	BDT_CLASS_PTR(GraphicsShader)
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
		MySimpleGeometryPass();
		virtual bool Init() override;

		GLSamplerUPtr m_materialSampler;
		ResourceHandle<GraphicsShader> m_geometryShaderStatic;
		ResourceHandle<GraphicsShader> m_geometryShaderSkinned;
	};
}
