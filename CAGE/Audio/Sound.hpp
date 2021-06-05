#pragma once

#include <SDL2/SDL_mixer.h>

namespace cage
{
	namespace audio
	{
		// A playable sound clip in CAGE. Stores the raw audio data but no playback information
		class Sound
		{
			Mix_Chunk* m_chunk;
		public:
			Sound(const char* filename);
			~Sound();
			Mix_Chunk* GetData();
		};
	}
}