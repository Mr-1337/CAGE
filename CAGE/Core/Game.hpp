#pragma once

#include "Window.hpp"

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
	private:
		// Loads each SDL library, throws an exception if any fail
		void initSDL();
		void unloadSDL();

	};
}