#include <iostream>
#include <exception>
#include <Glad/glad/glad.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "Game.hpp"

static void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

namespace cage
{
	Game::Game(const char* title, int argc, char** argv)
	{
		try
		{
			initSDL();
			m_window = new Window(title, 1024, 768);
			gladLoadGLLoader(SDL_GL_GetProcAddress);

			printf("Vendor:   %s\n", glGetString(GL_VENDOR));
			printf("Renderer: %s\n", glGetString(GL_RENDERER));
			printf("Version:  %s\n", glGetString(GL_VERSION));
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(MessageCallback, 0);
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	Game::~Game()
	{
		delete m_window;
	}

	void Game::initSDL()
	{

		// Base SDL

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			throw std::runtime_error("Failed to initialize SDL.");

		// SDL_image

		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if ((IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & imgFlags) != imgFlags)
			throw std::runtime_error("Failed to initialize SDL_image.");

		// SDL_mixer

		if (Mix_OpenAudio(44100, AUDIO_F32LSB, 8, 1024) == -1)
			throw std::runtime_error("Failed to OpenAudio with SDL_mixer.");

		int sndFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
		if ((Mix_Init(sndFlags) & sndFlags) != sndFlags)
			throw std::runtime_error("Failed to initialize SDL_mixer.");

		// SDL_ttf

		if (TTF_Init() != 0)
			throw std::runtime_error("Failed to initialize SDL_ttf.");

		// SDL_net

		if (SDLNet_Init() != 0)
			throw std::runtime_error("Failed to initialize SDL_net.");
	}

	void Game::unloadSDL()
	{
		SDLNet_Quit();
		TTF_Quit();
		Mix_CloseAudio();
		Mix_Quit();
		IMG_Quit();
		SDL_Quit();
	}
}