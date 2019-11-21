#pragma once

#include <stack>
#include "GameState.hpp"

namespace cage
{
	class StateMachine
	{
	public:
		StateMachine();
		void ProcessEvents();
		void Update(float delta);
		void Draw();

		void Push(GameState* state);
		bool Quit();
	private:

		bool m_quit = false;
		std::stack<GameState*> m_stateStack;
	};
}