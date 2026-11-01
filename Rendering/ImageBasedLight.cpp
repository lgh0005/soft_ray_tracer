#include "pch.h"
#include "ImageBasedLight.h"
#include "RenderTextureCube.h"
#include "RenderTexture2D.h"

namespace bdt
{
	namespace
	{
		constexpr uint32 EnvironmentSize = 2048;
		constexpr uint32 IrradianceSize = 32;
		constexpr uint32 PrefilterSize = 512;
		constexpr uint32 BRDFSize = 256;
		constexpr GLint SampleCount = 1024;
		constexpr uint32 WorkgroupSize = 8;

		// 조기 반환 시에도 호출자의 바인딩과 큐브 샘플링 상태를 복원한다.
		class IBLStateScope
		{
		public:
			IBLStateScope()
			{
				glGetIntegerv(GL_CURRENT_PROGRAM, &m_program);
				glGetIntegerv(GL_ACTIVE_TEXTURE, &m_activeTexture);
				glActiveTexture(GL_TEXTURE0);
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_texture2D);
				glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &m_textureCube);
				glActiveTexture(m_activeTexture);
				glGetIntegeri_v(GL_SAMPLER_BINDING, 0, &m_sampler);
				glGetIntegeri_v(GL_IMAGE_BINDING_NAME, 0, &m_image);
				glGetIntegeri_v(GL_IMAGE_BINDING_LEVEL, 0, &m_imageLevel);
				glGetIntegeri_v(GL_IMAGE_BINDING_LAYERED, 0, &m_imageLayered);
				glGetIntegeri_v(GL_IMAGE_BINDING_LAYER, 0, &m_imageLayer);
				glGetIntegeri_v(GL_IMAGE_BINDING_ACCESS, 0, &m_imageAccess);
				glGetIntegeri_v(GL_IMAGE_BINDING_FORMAT, 0, &m_imageFormat);
				m_seamless = glIsEnabled(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			}

			~IBLStateScope()
			{
				glUseProgram(m_program);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_texture2D);
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureCube);
				glActiveTexture(m_activeTexture);
				glBindSampler(0, m_sampler);
				glBindImageTexture(0, m_image, m_imageLevel, m_imageLayered, m_imageLayer, m_imageAccess, m_imageFormat);
				if (m_seamless) glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
				else glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			}

		private:
			GLint m_program{}, m_activeTexture{}, m_texture2D{}, m_textureCube{}, m_sampler{};
			GLint m_image{}, m_imageLevel{}, m_imageLayered{}, m_imageLayer{}, m_imageAccess{}, m_imageFormat{};
			GLboolean m_seamless{};
		};

		bool CheckGLError(cstr stage)
		{
			const GLenum error = glGetError();
			if (error == GL_NO_ERROR) return true;
			BDT_LOG_ERROR("ImageBasedLight: {} failed (GL error {}).", stage, error);
			return false;
		}

		uint32 GetMipLevels(uint32 size)
		{
			uint32 levels = 1;
			for (; size > 1; size >>= 1) ++levels;
			return levels;
		}

		// [TODO] 이후 UBO로 통일 고려 필요
		bool SetUniform(const ComputeShader& shader, cstr name, GLint value)
		{
			const uint32 program = shader.GetProgramID();
			const GLint location = glGetUniformLocation(program, name);
			if (location < 0) return false;
			glProgramUniform1i(program, location, value);
			return true;
		}

		void SetTextureMipRange(uint32 texture, uint32 mipLevels)
		{
			glTextureParameteri(texture, GL_TEXTURE_BASE_LEVEL, 0);
			glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipLevels - 1));
		}

		RenderTextureCubePtr CreateCube(uint32 size, uint32 mipLevels)
		{
			RenderTextureCubePtr texture = RenderTextureCube::Create(size, GL_RGBA16F, mipLevels);
			if (!texture) return nullptr;
			SetTextureMipRange(texture->GetTextureID(), mipLevels);
			if (!CheckGLError("cube allocation")) return nullptr;
			return texture;
		}

		void Dispatch(uint32 size, uint32 faces)
		{
			const uint32 groups = (size + WorkgroupSize - 1) / WorkgroupSize;
			glDispatchCompute(groups, groups, faces);
			// image 기록을 후속 텍스처 샘플링과 mip 생성/readback에 공개한다.
			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);
		}
	}

	ImageBasedLight::ImageBasedLight() = default;
	ImageBasedLight::~ImageBasedLight() = default;

	void ImageBasedLight::SetIntensity(float intensity)
	{
		m_intensity = std::max(intensity, 0.0f);
	}

	ImageBasedLightUPtr ImageBasedLight::Create(const Image2DPtr& source)
	{
		ImageBasedLightUPtr iblLight(new ImageBasedLight());
		if (!iblLight->Init(source)) return nullptr;
		return iblLight;
	}

	bool ImageBasedLight::Init(const Image2DPtr& source)
	{
		if (!source || !source->IsValid()) return false;

		m_hdrSampler = GLSampler::Create();
		m_cubeSampler = GLSampler::Create();
		if (!m_hdrSampler || !m_cubeSampler) return false;
		m_hdrSampler->SetWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
		m_hdrSampler->SetFilter(GL_LINEAR, GL_LINEAR);
		m_cubeSampler->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		m_cubeSampler->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		if (!CheckGLError("sampler allocation")) return false;

		m_source = source;

		// 요청만 등록한다. 컴파일 완료를 기다리거나 워커를 차단하지 않는다.
		equirectToCubeCompute = BDT_RESOURCE.LoadComputeShader
		(
			"IBL.EquirectToCube",
			{{ GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_ibl_equirect_to_cube.comp" }}
		);
		irradianceCompute = BDT_RESOURCE.LoadComputeShader
		(
			"IBL.Irradiance",
			{{ GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_ibl_irradiance.comp" }}
		);
		prefilterCompute = BDT_RESOURCE.LoadComputeShader
		(
			"IBL.Prefilter",
			{{ GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_ibl_prefilter.comp" }}
		);
		brdfLUTCompute = BDT_RESOURCE.LoadComputeShader
		(
			"IBL.BRDFLUT",
			{{ GL_COMPUTE_SHADER, "Assets/Default/Shaders/bdt_ibl_brdf_lut.comp" }}
		);

		return true;
	}

	bool ImageBasedLight::Generate()
	{
		if (!m_source || !m_source->IsValid() || !glfwGetCurrentContext()) return false;
		if (IsValid()) return true;

		// 로딩 중에는 다음 프레임에 재시도할 수 있도록 비차단으로 반환한다.
		if (!equirectToCubeCompute.IsReady() || !irradianceCompute.IsReady() ||
			!prefilterCompute.IsReady() || !brdfLUTCompute.IsReady()) return false;

		const auto equirectShader = equirectToCubeCompute.Get();
		const auto irradianceShader = irradianceCompute.Get();
		const auto prefilterShader = prefilterCompute.Get();
		const auto brdfShader = brdfLUTCompute.Get();
		if (!equirectShader || !irradianceShader || !prefilterShader || !brdfShader ||
			!equirectShader->IsValid() || !irradianceShader->IsValid() ||
			!prefilterShader->IsValid() || !brdfShader->IsValid()) return false;

		if (!CheckGLError("generation entry")) return false;
		GLint maxCubeSize = 0, maxTextureSize = 0;
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeSize);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		if (maxCubeSize < static_cast<GLint>(EnvironmentSize) ||
			maxCubeSize < static_cast<GLint>(IrradianceSize) ||
			maxCubeSize < static_cast<GLint>(PrefilterSize) ||
			maxTextureSize < static_cast<GLint>(BRDFSize)) return false;

		IBLStateScope stateScope;
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		RenderTextureCubePtr nextEnvironment, nextIrradiance, nextPrefiltered;
		RenderTexture2DPtr nextBRDFLUT;

		// 네 단계가 모두 성공한 경우에만 현재 IBL을 교체한다.
		if (!GenerateEnvironment(*equirectShader, nextEnvironment)) return false;
		if (!GenerateIrradiance(*irradianceShader, *nextEnvironment, nextIrradiance)) return false;
		if (!GeneratePrefiltered(*prefilterShader, *nextEnvironment, nextPrefiltered)) return false;
		if (!GenerateBRDFLUT(*brdfShader, nextBRDFLUT)) return false;

		environment = std::move(nextEnvironment);
		irradiance = std::move(nextIrradiance);
		prefiltered = std::move(nextPrefiltered);
		brdfLUT = std::move(nextBRDFLUT);
		return true;
	}

	bool ImageBasedLight::GenerateEnvironment(const ComputeShader& shader, RenderTextureCubePtr& output)
	{
		output = CreateCube(EnvironmentSize, GetMipLevels(EnvironmentSize));
		if (!output || !SetUniform(shader, "uSize", EnvironmentSize)) return false;
		shader.Bind();
		m_source->Bind(0);
		m_hdrSampler->Bind(0);
		glBindImageTexture(0, output->GetTextureID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		Dispatch(EnvironmentSize, 6);
		glGenerateTextureMipmap(output->GetTextureID());
		return CheckGLError("environment generation");
	}

	bool ImageBasedLight::GenerateIrradiance
	(
		const ComputeShader& shader,
		const RenderTextureCube& source,
		RenderTextureCubePtr& output
	)
	{
		output = CreateCube(IrradianceSize, 1);
		if (!output || !SetUniform(shader, "uSize", IrradianceSize) ||
			!SetUniform(shader, "uSamples", SampleCount)) return false;
		shader.Bind();
		source.Bind(0);
		m_cubeSampler->Bind(0);
		glBindImageTexture(0, output->GetTextureID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		Dispatch(IrradianceSize, 6);
		return CheckGLError("irradiance generation");
	}

	bool ImageBasedLight::GeneratePrefiltered
	(
		const ComputeShader& shader,
		const RenderTextureCube& source,
		RenderTextureCubePtr& output
	)
	{
		const uint32 mipLevels = GetMipLevels(PrefilterSize);
		output = CreateCube(PrefilterSize, mipLevels);
		if (!output || !SetUniform(shader, "uSamples", SampleCount)) return false;
		// [TODO] 이후 UBO로 통일 고려 필요
		const GLint roughnessLocation = glGetUniformLocation(shader.GetProgramID(), "uRoughness");
		if (roughnessLocation < 0) return false;
		shader.Bind();
		source.Bind(0);
		m_cubeSampler->Bind(0);
		for (uint32 mip = 0; mip < mipLevels; ++mip)
		{
			const uint32 size = glm::max(1u, PrefilterSize >> mip);
			if (!SetUniform(shader, "uSize", size)) return false;
			const float roughness = mipLevels > 1 ? static_cast<float>(mip) / (mipLevels - 1) : 0.0f;
			glProgramUniform1f(shader.GetProgramID(), roughnessLocation, roughness);
			glBindImageTexture(0, output->GetTextureID(), mip, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			Dispatch(size, 6);
			if (!CheckGLError("prefilter mip generation")) return false;
		}
		return true;
	}

	bool ImageBasedLight::GenerateBRDFLUT(const ComputeShader& shader, RenderTexture2DPtr& output)
	{
		output = RenderTexture2D::Create(BRDFSize, BRDFSize, GL_RG16F);
		if (!output) return false;
		SetTextureMipRange(output->GetTextureID(), 1);
		if (!SetUniform(shader, "uSize", BRDFSize) || !SetUniform(shader, "uSamples", SampleCount)) return false;
		shader.Bind();
		glBindImageTexture(0, output->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
		Dispatch(BRDFSize, 1);
		return CheckGLError("BRDF LUT generation");
	}

	bool ImageBasedLight::IsValid() const
	{
		return environment && environment->IsValid() && irradiance && irradiance->IsValid() &&
			prefiltered && prefiltered->IsValid() && brdfLUT && brdfLUT->IsValid();
	}
}
