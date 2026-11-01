#include "pch.h"
#include "MyWorld.h"

bdt::App g_application = bdt::App();

bdt::int32 main()
{
	// 1. App 초기화
	if (!g_application.Init())
		return -1;

	// 2. Level 구성 
	auto world = bdt::MyWorld::Create();
	auto pipeline = bdt::MyRTBruteForce::Create();

	// 3. Level 등록
	if (!g_application.RegisterLevel(std::move(world), std::move(pipeline)))
		return -1;

	// 4. Level 활성화
	g_application.ChangeLevel("MyWorld", "MyRTBruteForce");

	// 3. 메인 루프
	g_application.Update();
	
	// 4. 종료
	g_application.Clear();

	return 0;
}
