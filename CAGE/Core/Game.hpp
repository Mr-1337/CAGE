#pragma once

#include <random>

#include "Window.hpp"
#include "StateMachine.hpp"
#include <Version.hpp>
#include "../Graphics/Context/Context.hpp"
#include "../Graphics/Renderer/Renderer.hpp"
#include "../IO/Assets/TextureManager.hpp"
#include "../IO/Assets/SoundManager.hpp"
#include "../IO/Localization/Language.hpp"
#include "../IO/Logging/Logger.hpp"
#include "../Audio/PlaybackEngine.hpp"

namespace cage
{
	// A CAGE game. All games must inherit from this base and implement the methods.
	class Game
	{
	public:
		Game(const char* title, Version_t version = { 0, 0, 0 });
		~Game();

		void EngineInit();

		virtual void HandleCMDArgs(int argc, char** argv);
		virtual void Init() = 0;

		// Runs the main game loop
		virtual void Run() = 0;
		void Run2();
		virtual void Update(float frameTime) = 0;

		Window& GetWindow();
		TextureManager& GetTextureManager();
		SoundManager& GetSoundManager();
		Language& GetLanguage();
		graphics::Scene& GetScene();
		audio::PlaybackEngine& GetPlaybackEngine();

		std::mt19937& GetRNG();

	protected:
		std::unique_ptr<Window> m_window;

		// A finite state machine, facilitates switching between potentially vastly different application states
		StateMachine m_stateMachine;
		std::unique_ptr<io::InputManager> m_input;
		TextureManager m_textureManager;
		SoundManager m_soundManager;
		Language m_language;
		std::unique_ptr<graphics::Scene> m_scene;
		std::unique_ptr<graphics::Context> m_graphicsContext;

		audio::PlaybackEngine m_playbackEngine;

		const Version_t m_gameVersion;

	private:

		// Loads each SDL library, throws an exception if any fail
		void initSDL();
		void initGL();
		void initVulkan();

		// Handles events from the OS
		void processEvents();

		void unloadSDL();

		bool m_running;
		std::string m_title;
		std::mt19937 m_rng;
		io::Logger m_logger;
		vk::SurfaceKHR m_surface;
		std::unique_ptr<graphics::Renderer> m_renderer;
	};
}