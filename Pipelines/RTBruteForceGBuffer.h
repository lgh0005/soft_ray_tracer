#pragma once
#include <Rendering/IRenderTarget.h>

namespace bdt
{
	class RTBruteForceGBuffer : public IRenderTarget
	{
	public:
		virtual bool Init(uint32 width, uint32 height) override;
		virtual bool Resize(uint32 width, uint32 height) override;
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint32 GetWidth() const override;
		virtual uint32 GetHeight() const override;
	};
}