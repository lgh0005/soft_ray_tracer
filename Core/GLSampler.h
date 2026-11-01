#pragma once
#include "Defines.h"
#include "Types.h"
#include <glad/gl.h>

namespace bdt
{
	BDT_CLASS_PTR(GLSampler)

	class GLSampler
	{
		BDT_DISABLE_COPY(GLSampler)
		BDT_DISABLE_MOVE(GLSampler)

	public:
		~GLSampler();
		static GLSamplerUPtr Create();

	public:
		void Bind(uint32 textureUnit) const;
		static void Unbind(uint32 textureUnit);

	public:
		void SetWrapS(GLenum wrapMode);
		void SetWrapT(GLenum wrapMode);
		void SetWrapR(GLenum wrapMode);
		void SetWrap(GLenum wrapS, GLenum wrapT);
		void SetWrap(GLenum wrapS, GLenum wrapT, GLenum wrapR);

		void SetMinFilter(GLenum filter);
		void SetMagFilter(GLenum filter);
		void SetFilter(GLenum minFilter, GLenum magFilter);
		void SetMaxAnisotropy(float anisotropy);

		void SetBorderColor(const vec4& color);
		void SetCompareMode(GLenum mode);

	public:
		uint32 GetSamplerID() const;
		bool IsValid() const;

	private:
		GLSampler();
		uint32 m_samplerID{ 0 };
	};
}
