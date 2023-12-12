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
	Game::Game(const char* title, Version_t version) :
		m_running(true),
		m_language("en-US"),
		m_title(title),
		m_rng(1337),
		m_gameVersion(version)
	{

	}

	Game::~Game()
	{
		m_input = nullptr;
		m_graphicsContext->m_device->operator vk::Device().waitIdle();
		m_scene = nullptr;
		m_renderer = nullptr;
		m_graphicsContext->operator vk::Instance().destroySurfaceKHR(m_surface);
		m_graphicsContext = nullptr;
		unloadSDL();
	}

	void Game::HandleCMDArgs(int argc, char** argv)
	{

	}

	void Game::Run2()
	{
		using namespace std::chrono;
		duration<float> frameTime(1.f / 60.f);
		time_point<high_resolution_clock> startTime;

		while (m_running)
		{
			startTime = high_resolution_clock::now();
			processEvents();
			m_input->PollInput();
			m_renderer->m_Time += frameTime.count();
			Update(frameTime.count());
			m_scene->ComputeTransforms();
			m_renderer->Render(*m_scene);
			frameTime = (high_resolution_clock::now() - startTime);
		}
		io::Logger::Log("bye bye :(");
	}

	void Game::processEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			m_input->Raise(event);
			switch (event.type)
			{
			case SDL_QUIT:
				m_running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					auto newsize = std::make_pair(event.window.data1, event.window.data2);
					m_renderer->OnResize(newsize);
					m_scene->m_MainCam.SetWinSize(newsize);
				}
				break;
			}

		}
	}

#pragma region Getters

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

	graphics::Scene& Game::GetScene()
	{
		return *m_scene;
	}

	audio::PlaybackEngine& Game::GetPlaybackEngine()
	{
		return m_playbackEngine;
	}

	std::mt19937& Game::GetRNG()
	{
		return m_rng;
	}

#pragma endregion
#pragma region Init
	void Game::EngineInit()
	{
		const bool vulkan = true;
		io::Logger::Logf("Launching CAGE v{}.{}.{}", EngineVersion.major, EngineVersion.minor, EngineVersion.patch);
		std::vector<std::string> names;
		names =
		{
			"Considerably Average Game Engine",
			"Consistently Awkward Game Engine",
			"Categorically Atrocious Game Engine",
			"Cacophonous Audio Game Engine",
			"Ceaseless Assault of Grotesque Exceptions",
			"Chronically Abstract Game Engine",
			"C++ Agony and Grief Engine",
			"Chungus Amongus Game Engine",
			"Combative API Game Engine",
			"Complex And Generally Egregious",
			"Choose Another Game Engine",
			"Confusingly Atypical Game Engine",
			"Cordially Advertised Game Engine",
			"Criminally Authored Game Engine",
			"Consistently Avoided Game Engine",
			"Crimes Against Game Engines",
			"Creatively Aimless Game Engine",
			"Conventionally Astray Game Engine",
			"Clumsily Adapted Game Engine",
			"CPU Antagonizing Game Engine",
			"Carelessly Arranged Game Engine",
			"Crypto At Gamers' Expense"
		};

		m_rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
		io::Logger::Log(names[m_rng() % names.size()]);
		initSDL();
		io::Logger::Log("SDL started successfully");
		m_input = std::make_unique<io::InputManager>();

		m_window = std::make_unique<Window>(m_title.c_str(), Window::upair{ 1024u, 720u }, vulkan);
		if (vulkan)
		{
			initVulkan();
		}
		else
		{
			initGL();
			cage::Texture::s_MissingTexture = new cage::Texture(IMG_Load("Assets/Textures/missing.png"));
			cage::ui::UIElement::s_DefaultFont = new cage::Font("Assets/Fonts/consola.ttf", 18);
			cage::ui::UIElement::shader = std::make_shared<cage::SpriteShader>();
		}

		platform::Init();
		m_scene = std::make_unique<graphics::Scene>(*m_graphicsContext->m_device);
		m_scene->m_MainCam.SetWinSize(m_window->GetPixelSize());

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

	void Game::initVulkan()
	{
		auto windowExtensions = m_window->GetVulkanExtensions();
		m_graphicsContext = std::make_unique<graphics::Context>(m_title, m_gameVersion, windowExtensions);
		m_surface = m_window->GetVulkanSurface(*m_graphicsContext);
		vk::PhysicalDevice device = m_graphicsContext->m_device->GetPhysical();
		//cap.surfaceCapabilities.supportedCompositeAlpha

		m_renderer = std::make_unique<graphics::Renderer>(*m_graphicsContext->m_device, m_window->GetPixelSize(), m_surface);
		m_renderer->Init();
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
#pragma endregion

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