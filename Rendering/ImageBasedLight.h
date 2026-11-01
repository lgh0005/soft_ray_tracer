#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <Resources/Image2D.h>
#include <Resources/ResourceHandle.h>

namespace bdt
{
	BDT_CLASS_PTR(ComputeShader)
	BDT_CLASS_PTR(GLSampler)
	BDT_CLASS_PTR(RenderTextureCube)
	BDT_CLASS_PTR(RenderTexture2D)
	BDT_CLASS_PTR(ImageBasedLight)

	class ImageBasedLight
	{
		BDT_DISABLE_COPY(ImageBasedLight)
		BDT_DISABLE_MOVE(ImageBasedLight)

	public:
		~ImageBasedLight();
		static ImageBasedLightUPtr Create(const Image2DPtr& source);
		bool Generate();
		bool IsValid() const;
		float GetIntensity() const { return m_intensity; }
		void SetIntensity(float intensity);

		const RenderTextureCubePtr& GetEnvironment() const { return environment; }
		const RenderTextureCubePtr& GetIrradiance() const { return irradiance; }
		const RenderTextureCubePtr& GetPrefiltered() const { return prefiltered; }
		const RenderTexture2DPtr& GetBRDFLUT() const { return brdfLUT; }
			
	private:
		bool GenerateEnvironment(const ComputeShader& shader, RenderTextureCubePtr& output);
		bool GenerateIrradiance(const ComputeShader& shader, const RenderTextureCube& source, RenderTextureCubePtr& output);
		bool GeneratePrefiltered(const ComputeShader& shader, const RenderTextureCube& source, RenderTextureCubePtr& output);
		bool GenerateBRDFLUT(const ComputeShader& shader, RenderTexture2DPtr& output);

	private:
		ImageBasedLight();
		bool Init(const Image2DPtr& source);

		float m_intensity{ 1.5f };

		// Samplers
		GLSamplerUPtr m_hdrSampler;
		GLSamplerUPtr m_cubeSampler;

		// IBL Textures
		Image2DPtr m_source{ nullptr };
		RenderTextureCubePtr environment;
		RenderTextureCubePtr irradiance;
		RenderTextureCubePtr prefiltered;
		RenderTexture2DPtr   brdfLUT;

		// IBL Compute Shaders
		ResourceHandle<ComputeShader> equirectToCubeCompute;
		ResourceHandle<ComputeShader> irradianceCompute;
		ResourceHandle<ComputeShader> prefilterCompute;
		ResourceHandle<ComputeShader> brdfLUTCompute;
	};
}
