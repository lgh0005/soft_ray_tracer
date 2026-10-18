#pragma once
#include "Defines.h"
#include "Types.h"

struct GLFWwindow;

namespace bdt
{
	BDT_CLASS_PTR(GLWindow)

	class GLWindow
	{
		BDT_DISABLE_COPY(GLWindow)
		BDT_DISABLE_MOVE(GLWindow)

	public:
		static GLWindowUPtr Create();
		~GLWindow();

	public:
		void Destroy();
		void PollGLFWEvents();
		void SwapBuffers();
		bool ShouldClose() const;
		void RequestClose(bool shouldClose);

		int32 GetWidth() const { return m_width; }
		int32 GetHeight() const { return m_height; }
		int32 GetFramebufferWidth() const { return m_framebufferWidth; }
		int32 GetFramebufferHeight() const { return m_framebufferHeight; }
		GLFWwindow* GetHandle() const { return m_handle; }
		bool IsIconified() const { return m_isIconified; }
		bool IsFramebufferResized() const { return m_framebufferResized; }
		void ClearFramebufferResizeFlag() { m_framebufferResized = false; }

	private:

		// 윈도우 인스턴스 획득
		static GLWindow* GetGLFWWindow(GLFWwindow* window);

		// 윈도우 콜백
		static void HandleWindowPosition(GLFWwindow* window, int32 x, int32 y);
		static void HandleWindowSize(GLFWwindow* window, int32 width, int32 height);
		static void HandleWindowClose(GLFWwindow* window);
		static void HandleWindowRefresh(GLFWwindow* window);
		static void HandleWindowFocus(GLFWwindow* window, int32 focused);
		static void HandleWindowIconify(GLFWwindow* window, int32 iconified);
		static void HandleWindowMaximize(GLFWwindow* window, int32 maximized);
		static void HandleFramebufferSize(GLFWwindow* window, int32 width, int32 height);
		static void HandleContentScale(GLFWwindow* window, float xscale, float yscale);

		// 키보드 콜백
		static void HandleKey(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
		static void HandleChar(GLFWwindow* window, unsigned int codepoint);

		// 마우스 콜백
		static void HandleMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mods);
		static void HandleCursorPosition(GLFWwindow* window, double x, double y);
		static void HandleCursorEnter(GLFWwindow* window, int32 entered);
		static void HandleScroll(GLFWwindow* window, double xoffset, double yoffset);

		// 파일 콜백
		static void HandleFileDrop(GLFWwindow* window, int32 count, const char** paths);

	private:
		GLWindow();
		bool Init();

		GLFWwindow* m_handle      { nullptr };
		int32 m_width		      { 1280 };
		int32 m_height		      { 720 };
		String m_title			  { "bdt - Soft Ray Tracer" };

		int32 m_framebufferWidth  { 0 };
		int32 m_framebufferHeight { 0 };

		bool m_isIconified        { false };
		bool m_framebufferResized { false };
	};
}
