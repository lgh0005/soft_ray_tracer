#include "pch.h"
#include "RTBVHPostProcessingPass.h"

namespace bdt
{
    RTBVHPostProcessingPass::RTBVHPostProcessingPass() = default;

    RTBVHPostProcessingPass::~RTBVHPostProcessingPass()
    {
        Shutdown();
    }

    RTBVHPostProcessingPassUPtr RTBVHPostProcessingPass::Create()
    {
        RTBVHPostProcessingPassUPtr result(new RTBVHPostProcessingPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHPostProcessingPass::Init()
    {
        return true;
    }

    bool RTBVHPostProcessingPass::Execute(IRenderBus&)
    {
        return true;
    }

    void RTBVHPostProcessingPass::Shutdown()
    {
    }
}
