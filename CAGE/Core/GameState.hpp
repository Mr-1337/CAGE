#pragma once

#include <iostream>
#include "../IO/InputManager.hpp"

namespace cage
{
	class StateMachine;
	class Game;

	class GameState
	{
	public:

		// All the resource allocation for the game state should be done in the constructor if possible. A handle to the game instance is
		// passed around for shared resources such as the AssetManagers, the Renderer, and the PlaybackEngine
		GameState(Game& game);
		// Please free your resources!
		virtual ~GameState();

		// Use this to reconfigure any state when we swap back to this state from another
		virtual void OnRevealed() = 0;
		// Use thsi to configure any needed state when about to move to a new state
		virtual void OnHidden() = 0;

		virtual void ProcessEvents() = 0;
		virtual void Update(float dt) = 0;
		virtual void Draw() = 0;
		inline bool GetQuit() { return m_quit; }

		static StateMachine* s_stateMachine;

	protected:
		InputManager m_input;
		inline void quit() { m_quit = true; };
		inline Game& getGame() { return m_game; };

	private:
		bool m_quit = false;
		Game& m_game;

	};
}