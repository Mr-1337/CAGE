#pragma once

#include <stack>
#include "GameState.hpp"

namespace cage
{
	// The CAGE Finite State Machine
	class StateMachine
	{
	public:
		StateMachine();
		void ProcessEvents();
		void Update(float dt);
		void Draw();

		void Push(GameState* state);
		void Pop();

		bool Quit();
	private:

		bool m_quit = false;
		std::stack<GameState*> m_stateStack;
	};
}