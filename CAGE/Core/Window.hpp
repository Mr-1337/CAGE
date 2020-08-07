#pragma once

#include <SDL2/SDL.h>
#include <utility>

namespace cage
{
	// A CAGE Window. Wraps around SDL_Window to provide a cleaner API and also store additional stuff such as framebuffers
	class Window final
	{
	public:
		Window(const char* title, int w, int h);
		~Window();
		void SetPosition(int x, int y);
		std::pair<int, int> GetPosition() const;
		std::pair<int, int> GetSize() const;
		void SetSize(int w, int h);
		void MakeContextCurrent(SDL_GLContext context);
		inline SDL_GLContext GetGLContext() { return m_context; };
		inline void SwapBuffers() { SDL_GL_SwapWindow(m_rawHandle); }
	private:
		SDL_Window* m_rawHandle;
		SDL_GLContext m_context;
		int m_width, m_height;
	};
}