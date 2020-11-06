// This is where the madness starts


#include "UB.hpp"

// Entry Point
int main(int argc, char** argv)
{
    ub::UndefinedBehavior game(argc, argv);
    game.Run();

    // The Return
	return 0;
}

// But is this where it ends?