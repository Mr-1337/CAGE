#pragma once

#include "ScriptAction.hpp"

namespace ub
{
	class EntTeleportAction : public ScriptAction
	{
	public:
		EntTeleportAction(World* world, glm::vec2 destination);

		void Initialize() override;
		void Update(float dt) override;

	private:
		glm::vec2 m_destination;
	};
}