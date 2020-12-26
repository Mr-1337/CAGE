#pragma once

#include <chrono>
#include <SDL2/SDL_mixer.h>

#include "../CAGE/CAGE.hpp"
#include "GameStates/Sandbox.hpp"
#include "GameStates/MainMenu.hpp"

class STR : public cage::Game
{
public:
	STR(int argc, char** argv);
	~STR();

	void handleCMDArgs(int argc, char** argv) override;

	void Run() override;
private:
};