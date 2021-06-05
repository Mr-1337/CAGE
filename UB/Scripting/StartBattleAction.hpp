#pragma once

#include "ScriptAction.hpp"

namespace ub
{
	class StartBattleAction : public ScriptAction
	{
	public:
		StartBattleAction(World* world, Enemy* enemy);

		void Initialize() override;
		void Update(float dt) override;

	private:
		Enemy* m_enemy;
		bool m_started;
	};
}