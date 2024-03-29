#pragma once

#include <random>

#include "Window.hpp"
#include "StateMachine.hpp"
#include "../IO/Assets/TextureManager.hpp"
#include "../IO/Assets/SoundManager.hpp"
#include "../IO/Localization/Language.hpp"
#include "../Audio/PlaybackEngine.hpp"

namespace cage
{
	// A CAGE game. All games must inherit from this base and implement the methods.
	class Game
	{
	public:
		Game(const char* title);
		~Game();

		void EngineInit();

		virtual void HandleCMDArgs(int argc, char** argv);
		virtual void Init() = 0;

		// Runs the main game loop
		virtual void Run() = 0;

		Window& GetWindow();
		TextureManager& GetTextureManager();
		SoundManager& GetSoundManager();
		Language& GetLanguage();
		audio::PlaybackEngine& GetPlaybackEngine();

		std::mt19937& GetRNG();

	protected:
		std::unique_ptr<Window> m_window;


		// A finite state machine, facilitates switching between potentially vastly different application states
		StateMachine m_stateMachine;
		TextureManager m_textureManager;
		SoundManager m_soundManager;
		Language m_language;

		audio::PlaybackEngine m_playbackEngine;

	private:

		// Loads each SDL library, throws an exception if any fail
		void initSDL();

		void initGL();

		void unloadSDL();

		std::string m_title;
		std::mt19937 m_rng;

	};
}