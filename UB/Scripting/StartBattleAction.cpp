#include "StartBattleAction.hpp"

namespace ub
{
	StartBattleAction::StartBattleAction(World* world, Enemy* enemy) :
		ScriptAction(world), m_enemy(enemy), m_started(false)
	{
		m_complete = false;
	}

	void StartBattleAction::Initialize()
	{
		m_world->StartBattle(m_enemy);
		m_started = true;
	}

	void StartBattleAction::Update(float dt)
	{
		if (!m_world->InBattle() && m_started)
			m_complete = true;
	}
}