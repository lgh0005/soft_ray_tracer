#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Rendering/IRenderPass.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
    class ComputeShader;
    class RTBVHStorage;
    struct RTBVHRefitData;
    BDT_CLASS_PTR(RTBVHRefitPass)

    class RTBVHRefitPass : public IRenderPass
    {
        BDT_DISABLE_COPY(RTBVHRefitPass)
        BDT_DISABLE_MOVE(RTBVHRefitPass)

    public:
        virtual ~RTBVHRefitPass() override;
        static RTBVHRefitPassUPtr Create();
        virtual bool Execute(IRenderBus& bus) override;
        virtual void Shutdown() override;

        //[수정] Phase 5: keep the serial reference for identical-quality comparisons.
        void SetParallelInternalRefit(bool enabled);

    private:
        RTBVHRefitPass();
        virtual bool Init() override;
        bool AreShadersReady(bool refitBLAS) const;
        void RefitBLAS(RTBVHStorage& storage, const RTBVHRefitData& data);
        void RefitTLAS(const RTBVHRefitData& data);

    private:
        ResourceHandle<ComputeShader> m_leafRefitShader;
        ResourceHandle<ComputeShader> m_internalRefitShader;
        ResourceHandle<ComputeShader> m_levelRefitShader;
        ResourceHandle<ComputeShader> m_tlasLeafRefitShader;
        ResourceHandle<ComputeShader> m_tlasInternalRefitShader;

        bool m_parallelInternalRefit{ true };
    };
}
