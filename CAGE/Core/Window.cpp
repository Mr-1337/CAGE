#include "Window.hpp"
#include <Glad/glad/glad.h>

namespace cage
{

	Window::Window(const char* title, int w, int h) :
		m_width(w), m_height(h)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		m_rawHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		SDL_GLContext context = SDL_GL_CreateContext(m_rawHandle);
		SDL_assert(context != NULL);
		gladLoadGLLoader(SDL_GL_GetProcAddress);

		SDL_GL_SetSwapInterval(0);
	}

	Window::~Window()
	{
		SDL_DestroyWindow(m_rawHandle);
		m_rawHandle = nullptr;
	}

	void Window::SetPosition(int x, int y)
	{
		SDL_SetWindowPosition(m_rawHandle, x, y);
	}

	std::pair<int, int> Window::GetPosition() const
	{
		int x, y;
		SDL_GetWindowPosition(m_rawHandle, &x, &y);
		return { x, y };
	}

	std::pair<int, int> Window::GetSize() const
	{
		int w, h;
		SDL_GetWindowSize(m_rawHandle, &w, &h);
		return { w, h };
	}
}