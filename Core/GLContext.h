#pragma once

struct GLFWwindow;

namespace bdt
{
	class GLContext
	{
	public:
		virtual ~GLContext() = default;

		void MakeCurrent() const;
		void ReleaseCurrent() const;
		bool IsCurrent() const;

		virtual GLFWwindow* GetHandle() const = 0;
	
	protected:
		GLContext() = default;
	};
}
