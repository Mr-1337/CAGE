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
	cage::PlayGame<Shrektris>(argc, argv);
	std::cout << "It's not ogre, it's never ogre" << std::endl;

	struct k
	{
		float t = 0.0f;
	};

	std::vector<k> vec;
	vec.resize(20, { 0.0 });
	float tal = 0.0;

	std::for_each(vec.begin(), vec.end(), [&tal](auto& x) { x.t = tal += 3; });
	std::for_each(vec.crbegin(), vec.crend(), [](auto& x) { std::cout << x.t << std::endl; });

	std::cout << std::endl << std::endl;

	float target = 10.3;
	std::cout << "Searching for first frame less than " << target << std::endl;
	auto k1 = std::find_if(vec.crbegin(), vec.crend(), [&](const auto& k) { return k.t <= target; });
	
	if (k1 != vec.crend())
	{
		std::cout << k1->t << std::endl;
		k1--;
		std::cout << k1->t << std::endl;
	}

	return 0;
}
