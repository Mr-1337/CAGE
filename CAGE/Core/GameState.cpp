#include "GameState.hpp"

namespace cage
{
	GameState::GameState(Game& game) : m_game(game)
	{

		std::cout << "Created game state\n";
	}

	GameState::~GameState()
	{
		std::cout << "Destroyed game state\n";
	}
}