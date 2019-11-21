#include <iostream>
#include <thread>
#include "Shrektris/Shrektris.hpp"
#include "STR/STR.hpp"
#include "AFTR/AFTR.hpp"


template<typename T>
void PlayGame(int argc, char** argv)
{
	T game(argc, argv);
	game.Run();
}

int main(int argc, char** argv)
{
	PlayGame<STR>(argc, argv);
	std::cout << "It's not ogre, it's never ogre" << std::endl;
	return 0;
}
