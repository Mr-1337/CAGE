/*
 *	Welcome to the CAGE
 *
 *	Include the header for your game and make it the template argument for PlayGame<T>
 *
 */

#include <iostream>
#include "Shrektris/Shrektris.hpp"

template<typename T>
void PlayGame(int argc, char** argv)
{
	T game(argc, argv);
	game.Run();
}

int main(int argc, char** argv)
{
	PlayGame<Shrektris>(argc, argv);
	std::cout << "It's not ogre, it's never ogre" << std::endl;

	return 0;
}
