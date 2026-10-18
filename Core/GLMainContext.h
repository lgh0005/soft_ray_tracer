#pragma once
#include "Defines.h"
#include "GLContext.h"

namespace bdt
{
	BDT_CLASS_PTR(GLWindow)

	class GLMainContext final : public GLContext
	{
	public:
		explicit GLMainContext(GLWindow& window);
		virtual ~GLMainContext() override = default;

		GLFWwindow* GetHandle() const override;

	private:
		GLWindow* m_window{ nullptr };
	};
}
