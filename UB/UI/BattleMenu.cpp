#include <Graphics/UI/Text.hpp>
#include "BattleMenu.hpp"

namespace ub
{
	BattleMenu::BattleMenu(Battle& battle, cage::Game& game) : cage::ui::LayoutGroup(new cage::ui::ListLayout()), m_selected(0), m_count(0), m_game(game), m_battle(battle)
	{
		m_onClicks = {
			[this]() { m_battle.Attack(); },
			[this]() { std::cout << "1\n"; },
			[this]() { std::cout << "2\n"; },
			[this]() { m_battle.exit(); }
		};

		m_selector = std::make_shared<cage::ui::UIElement>();
		m_selector->SetActiveTexture(m_game.GetTextureManager().Get("Play.png"));
		m_selector->SetLocalMounting(cage::ui::MountPoint::CENTER_RIGHT);
		m_selector->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		m_selector->SetScale(0.5);
		AddAbsolute(m_selector);

		SetColor({ 0.2, 0.2, 0.2, 1.0 });

		auto names = { "Attack", "Magic", "Item", "Give up and die" };
		m_font = std::make_unique<cage::Font>("Assets/Fonts/consola.ttf", 36);

		for (auto n : names)
		{
			auto text = std::make_shared<cage::ui::Text>(*m_font);
			text->SetText(n);
			Add(text);
			m_count++;
		}

		m_selectSound = m_game.GetSoundManager().Get("XPExclamation.ogg");
		m_enterSound = m_game.GetSoundManager().Get("XPError.ogg");
		
		updateSelector();

		SetRelativeSizeAxes(cage::ui::Axis::VERTICAL);
		Resize({ GetSize().x, 1.0 });
	}

	void BattleMenu::updateSelector()
	{
		m_selected %= m_count;
		auto targetOption = GetLayoutChildren()[m_selected];
		m_selector->MoveTo(targetOption->GetPosition() + glm::vec2{ 0, targetOption->GetScaledSize().y } / 2.f);
		m_game.GetPlaybackEngine().Play(*m_selectSound, 1);
	}

	bool BattleMenu::HandleEvent(cage::Event& event)
	{
		if (cage::ui::LayoutGroup::HandleEvent(event))
			return true;
		else if (auto ke = std::get_if<cage::KeyDownEvent>(&event))
		{
			if (!ke->repeat)
			{
				switch (ke->keyCode)
				{
				case SDLK_w:
				case SDLK_UP:
					m_selected--;
					updateSelector();
					return true;
					break;
				case SDLK_s:
				case SDLK_DOWN:
					m_selected++;
					updateSelector();
					return true;
					break;
				case SDLK_RETURN:
					m_onClicks[m_selected]();
					m_game.GetPlaybackEngine().Play(*m_enterSound, 1);
					return true;
					break;
				}
			}
		}
		return false;
	}
}