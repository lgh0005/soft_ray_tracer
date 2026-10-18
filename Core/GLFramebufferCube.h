#pragma once
#include "Defines.h"
#include "Types.h"

namespace bdt
{
	BDT_CLASS_PTR(GLFramebufferCube)

	class GLFramebufferCube
	{
		BDT_DISABLE_COPY(GLFramebufferCube)
		BDT_DISABLE_MOVE(GLFramebufferCube)

    public:
        ~GLFramebufferCube();
        static GLFramebufferCubeUPtr Create(uint32 size);

    public:
        void Bind() const;
        static void Unbind();

        bool AttachColorFace(uint32 textureID, uint32 faceIndex, uint32 mipLevel = 0);
        bool AttachDepth(uint32 textureID);
        bool AttachDepthStencil(uint32 textureID);
        void SetDrawBuffers(uint32 count);
        bool IsComplete() const;

        uint32 GetFramebufferID() const;
        uint32 GetWidth() const;
        uint32 GetHeight() const;
        bool IsValid() const;

    private:
        GLFramebufferCube();
        bool Init(uint32 size);
        void Clear();

    private:
        uint32 m_framebufferID{ 0 };
        uint32 m_size{ 0 };
	};
}
