#pragma once

#include <SDL2/SDL_mixer.h>
#include "ScriptAction.hpp"

namespace ub
{
	class SoundAction : public ScriptAction
	{
		Mix_Chunk* m_chunk;
	public:
		SoundAction(World* world, const char* filename);
		~SoundAction();

		void Initialize() override;
		void Update(float dt) override;
	};
}