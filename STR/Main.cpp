#include <iostream>
#include "STR.hpp"

int main(int argc, char** argv)
{
	STR game(argc, argv);
	game.Run();
	std::cout << "It's not ogre, it's never ogre" << std::endl;

	return 0;
}
