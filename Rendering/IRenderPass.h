#pragma once
#include "IRenderBus.h"

namespace bdt
{
    class IRenderPass
    {
    public:
        virtual ~IRenderPass();
        virtual bool Init() = 0;
        // 정상 처리 또는 작업 없음은 true, 미준비 / 실패는 false.
        virtual bool Execute(IRenderBus& bus) = 0;
        virtual void Shutdown() = 0;
    };
}
