#include <iostream>
#include "STR.hpp"

#include <CAGE.hpp>

int main(int argc, char** argv)
{
	cage::PlayGame<STR>(argc, argv);
	std::cout << "It's not ogre, it's never ogre" << std::endl;

	return 0;
}
