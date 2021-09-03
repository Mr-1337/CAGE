#pragma once

#include <Util/Timer.hpp>

#include "Player.hpp"
#include "Enemy.hpp"

#include "World/WorldShader.hpp"
#include "UI/ProgressBar.hpp"

namespace ub
{
	class Battle
	{
		friend class BattleMenu;
	public:
		Battle(Player* player, Enemy* m_enemy, std::shared_ptr<WorldShader> shader);
		void Update(float dt);
		void Draw();
		void HandleEvents(cage::Event& event);
		inline bool Complete() { return m_complete; }
		inline std::shared_ptr<cage::ui::UIElement> GetPanel() { return m_battleUI; }
		
		void Attack();

		static cage::Game* s_Game;

	private:
		Player* m_player;
		Enemy* m_enemy;
		std::unique_ptr<cage::Font> m_font;
		std::shared_ptr<WorldShader> m_shader;

		void updateHPBar(Character* contestant);

		std::shared_ptr<cage::ui::UIElement> m_battleUI, m_actionMenu, m_bottomBar;
		std::map<Character*, std::shared_ptr<ProgressBar>> m_hpBars;
		glm::vec2 m_playerPos, m_enemyPos;
		bool m_complete;

		int m_turnCount;
		unsigned char m_currentTurn;

		std::normal_distribution<float> m_rng;
		cage::Timer* m_timer;

		void exit();
	};
}