#include "SoundAction.hpp"

namespace ub
{
	SoundAction::SoundAction(World* world, const char* filename) : ScriptAction(world)
	{
		m_chunk = Mix_LoadWAV(filename);
	}

	SoundAction::~SoundAction()
	{
	}
	
	void SoundAction::Initialize()
	{
		m_complete = true;
	}

	void SoundAction::Update(float dt)
	{

	}

}