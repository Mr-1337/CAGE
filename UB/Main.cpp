// This is where the madness starts

#include "UB.hpp"

// Entry Point
int main(int argc, char** argv)
{
    cage::PlayGame<ub::UndefinedBehavior>(argc, argv);

    // The Return
	return 0;
}

// But is this where it ends?