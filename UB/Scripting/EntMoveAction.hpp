#pragma once

#include "ScriptAction.hpp"

namespace ub
{
	class EntMoveAction : public ScriptAction
	{
	public:
		EntMoveAction(World* world, glm::vec2 m_start, glm::vec2 m_end, float duration);

		void Initialize() override;
		void Update(float dt) override;

	private:
		glm::vec2 m_start, m_end;
		float m_duration;
		float m_t, m_totalTime;
	};
}