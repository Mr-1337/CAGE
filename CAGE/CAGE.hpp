// CAGE.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "Core/Game.hpp"

namespace cage
{
	template <typename T>
	void PlayGame(int argc, char** argv)
	{
		T game;
		game.Run();
	}
}

// TODO: Reference additional headers your program requires here.
