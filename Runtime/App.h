#pragma once

namespace bdt
{
	BDT_CLASS_PTR(World)
	BDT_CLASS_PTR(IRenderPipeline)

	class App
	{
		BDT_DISABLE_COPY(App)
		BDT_DISABLE_MOVE(App)

	public:
		App()  = default;
		~App() = default;

	public:
		bool Init();
		void Update();
		void Render();
		void Clear();

	public:
		bool RegisterLevel
		(
			WorldUPtr world, 
			IRenderPipelineUPtr pipeline
		);

		void ChangeLevel
		(
			const String& worldTag, 
			const String& pipelineTag
		);

	/*==================================//
	//    Mesurement Debug Utilities    //
	//==================================*/
	private:
		void HandleDebugSnapshot();

	private:
		bool m_appInitialized { false };
	};
}
