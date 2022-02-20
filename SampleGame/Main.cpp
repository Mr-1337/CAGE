/*
 *	Welcome to the CAGE
 *
 *	Include the header for your game and make it the template argument for PlayGame<T>
 *
 */

#include <iostream>
#include "Shrektris/Shrektris.hpp"

#include <GLM/glm/glm.hpp>

int main(int argc, char** argv)
{
	cage::PlayGame<Shrektris>(argc, argv);
	std::cout << "It's not ogre, it's never ogre" << std::endl;
	return 0;
}
