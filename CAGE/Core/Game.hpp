#pragma once

#include "Window.hpp"
#include "StateMachine.hpp"

namespace cage
{
	// A CAGE game. All games must inherit from this base and implement the methods.
	class Game
	{
	public:
		Game(const char* title, int argc, char** argv);
		~Game();

		// Runs the main game loop
		virtual void Run() = 0;
	protected:
		Window* m_window;

		// A finite state machine, facilitates switching between potentially vastly different application states
		StateMachine m_stateMachine;
	private:
		// Loads each SDL library, throws an exception if any fail
		void initSDL();
		void unloadSDL();

	};
}