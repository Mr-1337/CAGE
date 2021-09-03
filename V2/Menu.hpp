#pragma once

#include <Core/GameState.hpp>

namespace v2
{
    class Menu : public cage::GameState
    {
	public:
		Menu(cage::Game& game);
		// Please free your resources!
		~Menu();

		// Use this to reconfigure any state when we swap back to this state from another
		void OnRevealed() override;
		// Use this to configure any needed state when about to move to a new state
		void OnHidden() override;

		void ProcessEvents() override;
		void Update(float dt) override;
		void Draw() override;
    };
}
