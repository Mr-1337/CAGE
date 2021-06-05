#pragma once

#include "../CAGE/Core/GameState.hpp"
#include "../World.hpp"

namespace ub
{
	class Gameplay : public cage::GameState
	{
	public:

		Gameplay(cage::Game& game);
		~Gameplay();

		void OnRevealed() override;
		void OnHidden() override;

		void ProcessEvents() override;
		void Update(float dt) override;
		void Draw() override;

	private:

		World* m_world;

	};
}