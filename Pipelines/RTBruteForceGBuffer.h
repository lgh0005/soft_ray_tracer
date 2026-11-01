#pragma once
#include <Rendering/IRenderTarget.h>

namespace bdt
{
	BDT_CLASS_PTR(RTBruteForceGBuffer)

	class RTBruteForceGBuffer : public IRenderTarget
	{
		BDT_DISABLE_COPY(RTBruteForceGBuffer)
		BDT_DISABLE_MOVE(RTBruteForceGBuffer)

	public:
		virtual ~RTBruteForceGBuffer() override;
		static RTBruteForceGBufferUPtr Create(uint32 width, uint32 height);

		virtual bool Resize(uint32 width, uint32 height) override;
		virtual void Bind() const override;
		virtual void Unbind() const override;
		void Clear() const;

		virtual uint32 GetWidth() const override;
		virtual uint32 GetHeight() const override;

	private:
		RTBruteForceGBuffer();
		virtual bool Init(uint32 width, uint32 height) override;

		uint32 m_width{ 0 };
		uint32 m_height{ 0 };
	};
}