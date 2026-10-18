#pragma once

namespace bdt
{
    BDT_CLASS_PTR(RenderScene)

    class IRenderBus
    {
    public: 
        virtual ~IRenderBus();
        virtual bool Resize(uint32 width, uint32 height) = 0;
        void SetRenderScene(const RenderScene* scene);
        const RenderScene* GetRenderScene() const;

        uint32 GetWidth()  const { return m_width; }
        uint32 GetHeight() const { return m_height; }

    protected:  
        IRenderBus();
        void SetSize(uint32 width, uint32 height)
        {
            m_width = width;
            m_height = height;
        }

    private:
        const RenderScene* m_renderScene{ nullptr };
        uint32 m_width{ 0 };
        uint32 m_height{ 0 };
    };
}