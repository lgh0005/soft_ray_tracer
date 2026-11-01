#include "pch.h"
#include "App.h"
#include "Runtime.h"

namespace bdt
{
	bool App::Init()
	{
		// 0. [디버그] 디버그 도구 초기화
		BDT_LOG_INIT();

		// 1. Window 및 OpenGL Context 생성
		if (!BDT_WINDOW.Init()) return false;

		// [디버그] 555
		BDT_GPU_PROFILE_INIT();

		// 2. Time
		BDT_TIME.Init();

		// 3. Input
		if (!BDT_INPUT.Init(BDT_WINDOW.GetNativeWindow()))
			return false;

		// 4. TaskManager
		const GLMainContext* mainContext = BDT_WINDOW.GetMainContext();
		if (!mainContext) return false;

		// 워커 스레드 생성 및 시작
		uint32 affinity = std::thread::hardware_concurrency();
		if (affinity == 0) affinity = 1;
		if (!BDT_TASK.Init(*mainContext, affinity)) return false;
		if (!BDT_TASK.Launch()) return false;

		// 5. ResourceManager
		// 별도의 Init()이 없으므로 초기화하지 않음

		// 6. InstanceManager
		if (!BDT_INSTANCE.Init())
			return false;

		// 7. RenderManager
		if (!BDT_RENDER.Init())
			return false;

		// 8. WorldManager
		// 별도의 Init()이 없으므로 초기화하지 않음

		// 9. GUIManager
		BDT_GUI.Init();

		m_appInitialized = true;
		return true;
	}

	void App::Update()
	{
		if (!m_appInitialized) 
			return;

		while (!BDT_WINDOW.ShouldClose())
		{
			// 1. Event Polling
			BDT_WINDOW.PollEvents();

			// 2. Time
			BDT_TIME.Update();

			// 3. Input
			BDT_INPUT.Update();

			// 4. World
			BDT_WORLD.Update(BDT_TIME.GetDeltaTime());

			// 5. Rendering
			Render();

			// 렌더링 결과를 버퍼 교환 전에 저장
			HandleDebugSnapshot();

			// 6. Swap Buffers
			BDT_WINDOW.Update();
		}
	}

	void App::HandleDebugSnapshot()
	{
		if (!BDT_KEYBOARD || !BDT_KEYBOARD->GetKeyDown(GLFW_KEY_ENTER)) return;

		String error;
		if (BDT_SNAPSHOT_SAVE(BDT_WINDOW.GetNativeWindow(), "snapshot.bmp", &error))
			BDT_LOG_INFO("Screenshot saved: snapshot.bmp");
		else BDT_LOG_ERROR("Screenshot failed: {}", error);
	}

	void App::Render()
	{
		// 1. Framebuffer Resize
		const uint32 width = BDT_WINDOW.GetFramebufferWidth();
		const uint32 height = BDT_WINDOW.GetFramebufferHeight();
		if (width <= 0 || height <= 0) return;

		// 2. World 카메라의 Viewport 갱신
		BDT_WORLD.Resize(width, height);
		BDT_WORLD.Submit();

		// 3. Framebuffer 크기가 변경되었다면 렌더 타깃 갱신
		if (BDT_WINDOW.IsFramebufferResized() && BDT_RENDER.GetCurrentPipeline())
		{
			if (!BDT_RENDER.Resize(width, height)) return;
			BDT_WINDOW.ClearFramebufferResizeFlag();
		}

		// 4. Rendering
		BDT_RENDER.Render(BDT_WORLD.GetRenderScene(), width, height);
		BDT_GPU_PROFILE_COLLECT();

		// 5. GUI Rendering
		BDT_GUI.Render();
	}

	void App::Clear()
	{
		// 1. TaskManager
		BDT_TASK.Shutdown();

		// 2. GUIManager
		BDT_GUI.Shutdown();

		// 3. WorldManager
		BDT_WORLD.Shutdown();

		// 4. RenderManager
		BDT_RENDER.Shutdown();

		// 5. InstanceManager
		BDT_INSTANCE.Shutdown();

		// 6. ResourceManager
		BDT_RESOURCE.Clear();

		// 7. InputManager
		BDT_INPUT.Shutdown();

		// 8. TimeManager
		BDT_TIME.Shutdown();

		// [디버그] OpenGL 컨텍스트가 살아 있을 때 GPU 타이머 정리
		BDT_GPU_PROFILE_CLEAR();

		// 9. WindowManager
		BDT_WINDOW.Shutdown();

		// [디버그] 다른 도구의 정리 로그 출력 후 로거 정리
		BDT_LOG_CLEAR();
	}

	bool App::RegisterLevel(WorldUPtr world, IRenderPipelineUPtr pipeline)
	{
		if (!m_appInitialized)
			return false;

		if (!world || !pipeline)
			return false;

		// 각각의 Tag 확인
		const String worldTag = world->GetTag();
		const String pipelineTag = pipeline->GetTag();

		// 중복 등록 방지
		if (BDT_WORLD.GetWorld(worldTag)) return false;
		if (BDT_RENDER.GetPipeline(pipelineTag)) return false;

		// 각 Manager에 소유권 이전
		BDT_WORLD.RegisterWorld(std::move(world));
		BDT_RENDER.RegisterPipeline(std::move(pipeline));

		return true;
	}

	void App::ChangeLevel
	(
		const String& worldTag,
		const String& pipelineTag
	)
	{
		if (!m_appInitialized)
			return;

		// 변경하려는 World와 Pipeline 확인
		World* world = BDT_WORLD.GetWorld(worldTag);
		IRenderPipeline* pipeline = BDT_RENDER.GetPipeline(pipelineTag);
		if (!world || !pipeline)
			return;

		// 두 Manager에 전환 예약 및 기하 정보 초기화
		BDT_WORLD.ChangeWorld(worldTag);
		BDT_RENDER.ChangePipeline(pipelineTag);
		BDT_INSTANCE.ClearInstances();
		BDT_INSTANCE.ClearGeometry();
	}
}
