#pragma once
#include "IRenderBus.h"

namespace bdt
{
    class IRenderPass
    {
    public:
        virtual ~IRenderPass();
        virtual bool Init() = 0;
        virtual void Execute(IRenderBus& bus) = 0;
        virtual void Shutdown() = 0;
    };
}