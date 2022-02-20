#include <Graphics/UI/ListLayout.hpp>
#include "Battle.hpp"
#include "UI/BattleMenu.hpp"

namespace ub
{
	cage::Game* Battle::s_Game = nullptr;

	Battle::Battle(Player* player, Enemy* enemy, std::shared_ptr<WorldShader> shader) : m_player(player), m_enemy(enemy), m_complete(false), m_shader(shader), m_turnCount(1), m_currentTurn(0), m_rng(5, 1), m_timer(nullptr), k_introTime(1.f), m_introTimer(0.f)
	{
		using cage::ui::MountPoint;

		//m_enemy->LoadTexture("battle.png");
		m_playerPos = player->GetPosition();
		m_enemyPos = enemy->GetPosition();

		m_battleUI = std::make_shared<cage::ui::UIElement>();
		m_battleUI->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		m_battleUI->SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);

		m_bottomBar = std::make_shared<cage::ui::UIElement>();
		m_bottomBar->SetParentMounting(MountPoint::BOTTOM_LEFT);
		m_bottomBar->SetLocalMounting(MountPoint::BOTTOM_LEFT);
		m_bottomBar->SetRelativeSizeAxes(cage::ui::Axis::BOTH);

		m_hpBars[m_player] = std::make_shared<ProgressBar>(glm::vec2{ 300.0f, 40.0f });
		m_hpBars[m_enemy] = std::make_shared<ProgressBar>(glm::vec2{ 300.0f, 40.0f });

		m_hpBars[m_player]->MoveTo({ 100, 560 });
		m_hpBars[m_enemy]->MoveTo({ 650, 445 });

		m_battleUI->Add(m_hpBars[m_player]);
		m_battleUI->Add(m_hpBars[m_enemy]);

		m_actionMenu = std::make_shared<BattleMenu>(*this, *s_Game);
		m_actionMenu->SetRelativePositionAxes(cage::ui::Axis::VERTICAL);
		m_actionMenu->SetRelativeSizeAxes(cage::ui::Axis::VERTICAL);
		m_actionMenu->MoveTo({ 300, 0.5 });
		m_actionMenu->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);

		m_battleUI->Add(m_bottomBar);
		m_bottomBar->Add(m_actionMenu);
		m_bottomBar->Resize({ 1.0, 0.25 });
		m_bottomBar->SetColor({ 0.f, 0.f, 1.0f, 1.0f });

		std::for_each(m_hpBars.begin(), m_hpBars.end(), [this](auto e) { updateHPBar(e.first); });

		auto changeTurn = [this]() {
			++m_currentTurn %= 2;
			std::cout << "Lap\n";
		};

		m_timer = new cage::Timer(std::chrono::duration<float>(2.0f), changeTurn);
		m_timer->Start();	

	}

	void Battle::Update(float dt)
	{
		m_timer->Update();
	}

	void Battle::Attack()
	{
		switch(m_currentTurn)
		{
		case 0:
			m_enemy->Damage(1);
			updateHPBar(m_enemy);
			if (m_enemy->GetHP() == 0)
			{
				m_enemy->LoadTexture("battle2.png");
			}
			break;
		case 1:
			m_player->Damage(std::round(m_rng(s_Game->GetRNG())));
			updateHPBar(m_player);
			break;
		}
	}

	void Battle::Draw()
	{
		//m_shader->Use();
		//m_player->Draw();
		//m_enemy->Draw();
	}

	void Battle::updateHPBar(Character* contestant)
	{
		m_hpBars[contestant]->SetProgress(static_cast<float>(contestant->GetHP()) / contestant->GetMaxHP());
	}

	void Battle::HandleEvents(cage::Event& event)
	{
		m_battleUI->HandleEvent(event);
		if (auto ke = std::get_if<cage::KeyDownEvent>(&event))
		{
			if (ke->keyCode == SDLK_ESCAPE && !ke->repeat)
			{
				exit();
			}
		}
	}

	void Battle::exit()
	{
		m_complete = true;
		m_player->SetPosition(m_playerPos);
		m_enemy->SetPosition(m_enemyPos);

		m_enemy->LoadTexture("Uber.png");
	}
}