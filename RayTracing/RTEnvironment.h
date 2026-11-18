#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Resources/ResourceHandle.h>
#include <Resources/Image2D.h>
#include "RTUniforms.h"

namespace bdt
{
    class ImageBasedLight;
    class ComputeShader;
    BDT_CLASS_PTR(GLBuffer)
    BDT_CLASS_PTR(GLSampler)
    BDT_CLASS_PTR(RenderTexture2D)
    BDT_CLASS_PTR(RTEnvironment)

    class RTEnvironment
    {
        BDT_DISABLE_COPY(RTEnvironment)
        BDT_DISABLE_MOVE(RTEnvironment)
    public:
        ~RTEnvironment();
        static RTEnvironmentUPtr Create();
        bool Prepare(const ImageBasedLight* light, const RTRenderSettings& settings, bool& changed);
        void Bind() const;
        void Invalidate();
        void Shutdown();
    private:
        RTEnvironment();
        bool Init();
        bool BuildDistribution();
        struct alignas(16) EnvironmentData
        {
            vec4 transform{ 1.0f, 0.0f, 0.0f, 0.0f }; // cos(yaw), sin(yaw), intensity, ready
            uvec4 distribution{ 256u, 128u, 0u, 0u };
        };
        EnvironmentData m_data{};
        Image2DPtr m_source;
        uint32 m_sourceTexture{ 0 };
        bool m_distributionDirty{ true };
        GLBufferUPtr m_buffer;
        GLSamplerUPtr m_environmentSampler;
        GLSamplerUPtr m_distributionSampler;
        RenderTexture2DUPtr m_distribution;
        ResourceHandle<ComputeShader> m_rowsShader;
        ResourceHandle<ComputeShader> m_marginalShader;
    };
}
