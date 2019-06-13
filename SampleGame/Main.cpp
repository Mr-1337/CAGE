#include <iostream>
#include <SDL2/SDL.h>
#include <GLM/glm/glm.hpp>
#include "../CAGE/CAGE.h"

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
	
	SDL_Event e;
	bool running = true;
	
	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = false;
			}
		}
	}
	return 0;
}