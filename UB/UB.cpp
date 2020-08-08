#include "UB.hpp"

namespace ub
{
	UndefinedBehavior::UndefinedBehavior(int argc, char** argv) : cage::Game("Undefined Behavior", argc, argv)
	{

	}

	UndefinedBehavior::~UndefinedBehavior()
	{

	}

	void UndefinedBehavior::Run()
	{
		SDL_Delay(5000);
	}


}