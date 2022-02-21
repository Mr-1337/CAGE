#include <iostream>
#include <stdexcept>
#include <Glad/glad/glad.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>

#include "Game.hpp"
#include "Platform.hpp"
#include "Graphics/UI/UIElement.hpp"

static void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::string typeString;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		typeString = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeString = "Deprecated Behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeString = "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeString = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeString = "Performance";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeString = "Other";
		break;
	default:
		typeString = "Unknown message type";
	}

	std::string severityString;
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		severityString = "High";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severityString = "Medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		severityString = "Low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		break;
	default:
		severityString = "Unknown severity";
	}

	if (!severityString.empty())
	{
		std::cerr << "GL Callback\n";
		std::cerr << "Type: " << typeString << '\n';
		std::cerr << "Severity: " << severityString << '\n';
		std::cerr << "Message: " << message << '\n' << std::endl;
	}
}

namespace cage
{
	Game::Game(const char* title) :
		m_language("en-US"),
		m_title(title),
		m_rng(1337)
	{
		
	}

	void Game::HandleCMDArgs(int argc, char** argv)
	{

	}

	void Game::EngineInit()
	{
		std::cout << "Initializing CAGE, hang tight!" << std::endl;
		std::vector<std::string> names;
		names = {
			"Considerably Average Game Engine",
			"Consistently Awful Game Engine",
			"Categorically Atrocious Game Engine",
			"Choose Another Game Engine",
			"Confusingly Atypical Game Engine",
			"Contentious Architecture Game Engine",
			"Conceptually Abysmal Game Engine",
			"Cordially Advertised Game Engine",
			"Completely Antithetical Game Engine",
			"Criminal Authored Game Engine",
			"Consistently Avoided Game Engine",
			"Crimes Against Game Engines",
			"Creatively Aimless Game Engine",
			"Conventionally Astray Game Engine",
			"Clumsily Adapted Game Engine",
			"Computer Antagonizing Game Engine",
			"Carelessly Authored Game Engine",
			"Crypto-jacking At Gamers' Expense"
		};

		initSDL();
		m_window = std::make_unique<Window>(m_title.c_str(), 1024, 720);
		initGL();

		platform::Init();
		cage::Texture::s_MissingTexture = new cage::Texture(IMG_Load("Assets/Textures/missing.png"));
		cage::ui::UIElement::s_DefaultFont = new cage::Font("Assets/Fonts/consola.ttf", 18);
		cage::ui::UIElement::shader = std::make_shared<cage::SpriteShader>();
	}

	Game::~Game()
	{
		unloadSDL();
	}

	Window& Game::GetWindow()
	{
		return *m_window;
	}

	TextureManager& Game::GetTextureManager()
	{
		return m_textureManager;
	}

	SoundManager& Game::GetSoundManager()
	{
		return m_soundManager;
	}

	Language& Game::GetLanguage()
	{
		return m_language;
	}

	audio::PlaybackEngine& Game::GetPlaybackEngine()
	{
		return m_playbackEngine;
	}

	std::mt19937& Game::GetRNG()
	{
		return m_rng;
	}

	void Game::initGL()
	{
		//gladLoadGLLoader(SDL_GL_GetProcAddress);

		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n", glGetString(GL_VERSION));
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
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

		if (Mix_OpenAudio(44100, AUDIO_F32LSB, 2, 1024) == -1)
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