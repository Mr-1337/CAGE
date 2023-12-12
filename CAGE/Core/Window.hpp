#pragma once

#include <SDL2/SDL.h>
#include <utility>

namespace cage
{
	/* 
		A CAGE Window.Wraps around SDL_Window to provide a cleaner API.
		A Window purely handles window management, there is no application state or rendering info specifically tied to the window class.
		This enables an application to possibly create multiple windows.
	*/
	class Window final
	{
	public:

		using upair = std::pair<unsigned int, unsigned int>;

		Window(const char* title, upair size, bool vulkan);
		~Window();
		void SetPosition(unsigned int x, unsigned int y);
		upair GetPosition() const;
		upair GetSize() const;
		void SetIcon(const char* path);
		void SetSize(upair size);
		void SetOpacity(float opacity);
		void MakeContextCurrent(SDL_GLContext context);
		inline SDL_GLContext GetGLContext() { return m_context; };

		vk::SurfaceKHR GetVulkanSurface(vk::Instance instance);
		std::vector <const char*> GetVulkanExtensions() const;
		upair GetPixelSize() const;

		inline void SwapBuffers() { SDL_GL_SwapWindow(m_rawHandle); }
	private:
		upair m_size;
		SDL_Window* m_rawHandle;
		SDL_GLContext m_context;
	};
}