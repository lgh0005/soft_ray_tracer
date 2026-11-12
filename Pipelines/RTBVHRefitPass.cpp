#include "pch.h"
#include "RTBVHRefitPass.h"

namespace bdt
{
    RTBVHRefitPass::RTBVHRefitPass() = default;

    RTBVHRefitPass::~RTBVHRefitPass()
    {
        Shutdown();
    }

    RTBVHRefitPassUPtr RTBVHRefitPass::Create()
    {
        RTBVHRefitPassUPtr result(new RTBVHRefitPass());
        if (!result->Init()) return nullptr;
        return result;
    }

    bool RTBVHRefitPass::Init()
    {
        return true;
    }

    void RTBVHRefitPass::Execute(IRenderBus& bus)
    {
    }

    void RTBVHRefitPass::Shutdown()
    {
    }
}
