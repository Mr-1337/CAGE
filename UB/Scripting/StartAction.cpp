#include "StartAction.hpp"

namespace ub
{
	StartAction::StartAction(World* world) : ScriptAction(world)
	{

	}

	void StartAction::Initialize()
	{

	}

	void StartAction::Update(float dt)
	{
		m_complete = true;
	}
}