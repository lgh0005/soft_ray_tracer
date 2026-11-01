#include "pch.h"
#include "MyRTBruteForce.h"

namespace bdt
{
	MyRTBruteForce::MyRTBruteForce() : IRenderPipeline("MyRTBruteForce") { }

	bool MyRTBruteForce::Init()
	{
		return false;
	}

	bool MyRTBruteForce::Resize(uint32 width, uint32 height)
	{
		return false;
	}

	void MyRTBruteForce::Render(const RenderScene& scene, uint32 width, uint32 height)
	{
	}

	void MyRTBruteForce::Shutdown()
	{
	}
}