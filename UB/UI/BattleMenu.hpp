#pragma once

#include <functional>
#include <IO/Assets/TextureManager.hpp>
#include <Graphics/UI/LayoutGroup.hpp>
#include <Graphics/UI/ListLayout.hpp>
#include <Core/Game.hpp>

#include "../Battle.hpp"

namespace ub
{
	class BattleMenu : public cage::ui::LayoutGroup
	{
	public:
		BattleMenu(Battle& battle, cage::Game& game);
		bool HandleEvent(cage::Event& event) override;
	private:
		unsigned int m_selected, m_count;
		std::vector<std::function<void(void)>> m_onClicks;
		Child m_selector;
		cage::Game& m_game;
		std::unique_ptr<cage::Font> m_font;
		Battle& m_battle;
		std::shared_ptr<cage::audio::Sound> m_selectSound, m_enterSound;

		void updateSelector();
	};
}