#include "EntMoveAction.hpp"

namespace ub
{
	EntMoveAction::EntMoveAction(World* world, glm::vec2 start, glm::vec2 end, float duration) :
		ScriptAction(world),
		m_start(start),
		m_end(end),
		m_duration(duration),
		m_t(0),
		m_totalTime(0)
	{

	}

	void EntMoveAction::Initialize()
	{

	}

	void EntMoveAction::Update(float dt)
	{
		m_totalTime += dt;
		m_t = glm::min(m_totalTime / m_duration, 1.f);

		glm::vec2 pos = m_start * (1.f - m_t) + m_end * m_t;
		m_world->MoveEnt(1, pos);

		if (m_t >= 1.f)
			m_complete = true;
	}
}