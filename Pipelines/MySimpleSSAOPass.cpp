#include "pch.h"
#include "MySimpleSSAOPass.h"
//#include "MySimpleBus.h"
//#include "MySimpleGBuffer.h"
//#include <random>
//#include <cmath>
//
//namespace bdt
//{
//	MySimpleSSAOPass::~MySimpleSSAOPass() { Shutdown(); }
//	MySimpleSSAOPassUPtr MySimpleSSAOPass::Create()
//	{
//		auto pass = std::make_unique<MySimpleSSAOPass>();
//		return pass->Init() ? std::move(pass) : nullptr;
//	}
//
//	bool MySimpleSSAOPass::Init()
//	{
//		if (m_initialized) return true;
//		m_shader = BDT_RESOURCE.LoadGraphicsShader("MySimple.SSAO",
//			{ { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_output_pass.vert" },
//			  { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_ssao.frag" } });
//		m_blurShader = BDT_RESOURCE.LoadGraphicsShader("MySimple.SSAOBlur",
//			{ { GL_VERTEX_SHADER, "Assets/Default/Shaders/bdt_simple_output_pass.vert" },
//			  { GL_FRAGMENT_SHADER, "Assets/Default/Shaders/bdt_simple_ssao_blur.frag" } });
//
//		// 참조 구현과 같은 반구 커널. 고정 seed로 실행마다 같은 결과를 만든다.
//		std::mt19937 generator(0);
//		std::uniform_real_distribution<float> random(0.0f, 1.0f);
//		for (usize i = 0; i < m_kernel.size(); ++i)
//		{
//			vec3 sample(random(generator) * 2.0f - 1.0f, random(generator) * 2.0f - 1.0f, random(generator));
//			const float length = glm::length(sample);
//			sample = length > 1e-6f ? sample / length : vec3(0.0f, 0.0f, 1.0f);
//			const float scale = static_cast<float>(i) / static_cast<float>(m_kernel.size());
//			m_kernel[i] = sample * random(generator) * (0.1f + 0.9f * scale * scale);
//		}
//		auto noise = RenderTexture2D::Create(4, 4, GL_RGBA16F);
//		if (!noise) return false;
//		Array<vec4, 16> values{};
//		for (auto& value : values) value = vec4(random(generator) * 2.0f - 1.0f, random(generator) * 2.0f - 1.0f, 0.0f, 0.0f);
//		const GLuint id = noise->GetTextureID();
//		GLint unpackBuffer = 0, alignment = 0, rowLength = 0, skipRows = 0, skipPixels = 0, swapBytes = 0;
//		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &unpackBuffer);
//		glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
//		glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowLength);
//		glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
//		glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
//		glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapBytes);
//		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
//		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
//		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
//		glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
//		glTextureSubImage2D(id, 0, 0, 0, 4, 4, GL_RGBA, GL_FLOAT, values.data());
//		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpackBuffer);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
//		glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);
//		glPixelStorei(GL_UNPACK_SKIP_ROWS, skipRows);
//		glPixelStorei(GL_UNPACK_SKIP_PIXELS, skipPixels);
//		glPixelStorei(GL_UNPACK_SWAP_BYTES, swapBytes);
//		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		m_noise = std::move(noise);
//		m_initialized = true;
//		return true;
//	}
//
//	void MySimpleSSAOPass::Execute(IRenderBus& bus)
//	{
//		//auto* simpleBus = dynamic_cast<MySimpleBus*>(&bus);
//		//if (!simpleBus) return;
//		//simpleBus->SetSSAOReady(false);
//		//if (!m_initialized || !simpleBus->GetRenderScene()) return;
//		//auto* gbuffer = simpleBus->GetGBuffer();
//		//const uint32 width = bus.GetWidth(), height = bus.GetHeight();
//		//if (!gbuffer || !gbuffer->IsValid() || !width || !height) return;
//		//auto shader = m_shader.Get(), blur = m_blurShader.Get();
//		//if (!shader || !blur || !shader->IsValid() || !blur->IsValid()) return;
//		//auto* sampler = BDT_RENDER.GetNearestClampSampler();
//		//if (!sampler || !BDT_RENDER.GetScreenVAO() || !BDT_RENDER.GetScreenMesh()) return;
//		//MySimplePassState state;
//		//if (!m_raw.Resize(width, height, GL_R16F) ||
//		//	!simpleBus->GetSSAOTarget().Resize(width, height, GL_R16F) ||
//		//	!m_screen.Resize(width, height)) return;
//		//MySimplePassState::Fullscreen(width, height);
//		//const GLuint program = shader->GetProgramID();
//		//const auto& camera = simpleBus->GetRenderScene()->GetCamera();
//		//glProgramUniformMatrix4fv(program, glGetUniformLocation(program, "uView"), 1, GL_FALSE, &camera.view[0][0]);
//		//glProgramUniformMatrix4fv(program, glGetUniformLocation(program, "uProjection"), 1, GL_FALSE, &camera.projection[0][0]);
//		//// GLM vec3 정렬 설정과 무관하도록 스칼라 배열로 전송한다.
//		//Array<float, 64 * 3> samples{};
//		//for (usize i = 0; i < m_kernel.size(); ++i)
//		//	for (int j = 0; j < 3; ++j) samples[i * 3 + j] = m_kernel[i][j];
//		//glProgramUniform3fv(program, glGetUniformLocation(program, "uSamples[0]"), 64, samples.data());
//		//glProgramUniform1f(program, glGetUniformLocation(program, "uRadius"), m_radius);
//		//glProgramUniform1f(program, glGetUniformLocation(program, "uBias"), m_bias);
//		//gbuffer->GetTexture(EGBufferTextureSlot::Position)->Bind(0);
//		//gbuffer->GetTexture(EGBufferTextureSlot::Normal)->Bind(1);
//		//m_noise->Bind(2);
//		//glBindSampler(0, sampler->GetSamplerID());
//		//glBindSampler(1, sampler->GetSamplerID());
//		//glBindSampler(2, 0); // 노이즈의 REPEAT/NEAREST 텍스처 설정 사용
//		//m_raw.Bind();
//		//shader->Bind();
//		//m_screen.Draw();
//
//		//simpleBus->GetSSAOTarget().Bind();
//		//m_raw.GetColor()->Bind(0);
//		//blur->Bind();
//		//m_screen.Draw();
//		//simpleBus->SetSSAOReady(true);
//	}
//
//	void MySimpleSSAOPass::SetRadius(float value)
//	{
//		if (std::isfinite(value)) m_radius = std::max(value, 0.001f);
//	}
//	void MySimpleSSAOPass::SetBias(float value)
//	{
//		if (std::isfinite(value)) m_bias = std::max(value, 0.0f);
//	}
//	void MySimpleSSAOPass::Shutdown()
//	{
//		m_raw.Clear();
//		m_noise.reset();
//		m_shader = {};
//		m_blurShader = {};
//		m_initialized = false;
//	}
//}
