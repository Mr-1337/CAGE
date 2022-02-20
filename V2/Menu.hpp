#pragma once

#include <Core/GameState.hpp>
#include "UI/MenuButton.hpp"
#include "UI/SongWheel.hpp"
#include "Playfield.hpp"

namespace v2
{
    class Menu : public cage::GameState
    {
		cage::ui::UIElement m_root;
		std::shared_ptr<SongWheel> m_songWheel;
		std::shared_ptr<cage::ui::UIElement> m_scrollPane;
		Playfield* m_playfield;
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
