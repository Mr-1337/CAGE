#include "EntTeleportAction.hpp"

namespace ub
{
	EntTeleportAction::EntTeleportAction(World* world, glm::vec2 destination) : 
		ScriptAction(world),
		m_destination(destination)
	{

	}

	void EntTeleportAction::Initialize()
	{

	}

	void EntTeleportAction::Update(float dt)
	{
		m_world->MoveEnt(1, m_destination);
		m_complete = true;
	}
}