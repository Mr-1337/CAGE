#include "Window.hpp"
#include <Glad/glad/glad.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>

namespace cage
{

	Window::Window(const char* title, upair size, bool vulkan) :
		m_size(size)
	{
		if (!vulkan)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

			m_rawHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_size.first, m_size.second, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);
			m_context = SDL_GL_CreateContext(m_rawHandle);

			SDL_assert(m_context != NULL);

			gladLoadGLLoader(SDL_GL_GetProcAddress);

			SDL_GL_SetSwapInterval(1);
		}
		else
		{
			m_rawHandle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_size.first, m_size.second, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);
			SDL_SetWindowMinimumSize(m_rawHandle, 10, 10);
			//SDL_SysWMinfo info;
			//SDL_VERSION(&info.version);
			//SDL_GetWindowWMInfo(m_rawHandle, &info);

			//HWND native = info.info.win.window;

			//DWORD style = GetWindowLong(native, GWL_STYLE);
			//style &= ~WS_OVERLAPPEDWINDOW;
			//style |= WS_POPUP;
			//SetWindowLong(native, GWL_STYLE, style);

			//DWM_BLURBEHIND bb = { 0 };
			//bb.dwFlags = DWM_BB_ENABLE;
			//bb.fEnable = true;
			//bb.hRgnBlur = NULL;
			//DwmEnableBlurBehindWindow(native, &bb);

			//MARGINS margins = { -1 };
			//DwmExtendFrameIntoClientArea(native, &margins);
		}
	}

	Window::~Window()
	{
		SDL_GL_DeleteContext(m_context);
		SDL_DestroyWindow(m_rawHandle);
		m_rawHandle = nullptr;
	}

	vk::SurfaceKHR Window::GetVulkanSurface(vk::Instance instance)
	{
		VkSurfaceKHR surface;
		if (!SDL_Vulkan_CreateSurface(m_rawHandle, instance, &surface))
			throw std::runtime_error("Failed to create Vulkan window surface!");
		return surface;
	}

	std::vector<const char*> Window::GetVulkanExtensions() const
	{
		unsigned int count = 0;
		SDL_Vulkan_GetInstanceExtensions(m_rawHandle, &count, nullptr);
		std::vector<const char*> extensions(count);
		if (!SDL_Vulkan_GetInstanceExtensions(m_rawHandle, &count, extensions.data()))
			throw std::runtime_error("Failed to get Vulkan instance extensions from window!");
		return extensions;
	}

	Window::upair Window::GetPixelSize() const
	{
		std::pair<int, int> size;
		SDL_Vulkan_GetDrawableSize(m_rawHandle, &size.first, &size.second);
		return size;
	}

	void Window::MakeContextCurrent(SDL_GLContext context)
	{
		SDL_GL_MakeCurrent(m_rawHandle, context);
	}

	void Window::SetIcon(const char* path)
	{
		auto icon = IMG_Load(path);
		SDL_SetWindowIcon(m_rawHandle, icon);
		SDL_FreeSurface(icon);
	}

	void Window::SetPosition(unsigned int x, unsigned int y)
	{
		SDL_SetWindowPosition(m_rawHandle, x, y);
	}


	Window::upair Window::GetPosition() const
	{
		int x, y;
		SDL_GetWindowPosition(m_rawHandle, &x, &y);
		return { x, y };
	}

	void Window::SetSize(upair size)
	{
		SDL_SetWindowSize(m_rawHandle, size.first, size.second);
	}

	Window::upair Window::GetSize() const
	{
		int w, h;
		SDL_GetWindowSize(m_rawHandle, &w, &h);
		return { w, h };
	}

	void Window::SetOpacity(float opacity)
	{
		SDL_SetWindowOpacity(m_rawHandle, opacity);
	}
}