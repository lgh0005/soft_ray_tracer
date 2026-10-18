#pragma once
#include "RenderScene.h"

namespace bdt
{
    class IRenderPipeline
    {
    public:
        virtual ~IRenderPipeline();

        virtual bool Init() = 0;
        virtual bool Resize(uint32 width, uint32 height) = 0;
        virtual void Render(const RenderScene& scene, uint32 width, uint32 height) = 0;
        virtual void Shutdown() = 0;
        const String& GetTag() const { return m_tag; }

    protected:
        explicit IRenderPipeline(const String& tag);

    private:
        String m_tag;
    };
}