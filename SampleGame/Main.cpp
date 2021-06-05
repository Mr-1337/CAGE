/*
 *	Welcome to the CAGE
 *
 *	Include the header for your game and make it the template argument for PlayGame<T>
 *
 */

#include <iostream>
#include <SDL2/SDL.h>
#include "Shrektris/Shrektris.hpp"
#include "../CAGE/CAGE.hpp"

int main(int argc, char** argv)
{
	
	char input;
	bool vrMode;

	std::cout << "Do you want to play in VR mode? [y/n]" << std::endl;

	do
	{
		std::cin >> input;
		switch (input)
		{
		case 'n':
			vrMode = false;
			break;
		case 'y':
			vrMode = true;
			break;
		default:
			std::cout << "Invalid input" << std::endl;
		}

	} while (input != 'n' && input != 'y');

	cage::PlayGame<Shrektris>(argc, argv);
	std::cout << "It's not ogre, it's never ogre" << std::endl;

	return 0;
}
