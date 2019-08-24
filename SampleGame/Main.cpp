#include <iostream>
#include <thread>
#include "Shrektris.hpp"
#include "STR.hpp"

int main(int argc, char** argv)
{

	//Shrektris game(argc, argv);
	STR game(argc, argv);

	game.Run();

	return 0;
}
