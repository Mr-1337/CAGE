#pragma once

#include <SDL2/SDL_mixer.h>
#include <GLM/glm/glm.hpp>
#include "Sound.hpp"

namespace cage
{
	namespace audio
	{
		// The master audio playback engine. All audio is processed through this class.
		class PlaybackEngine
		{
		public:
			PlaybackEngine()
			{

			}
			~PlaybackEngine()
			{

			}

			void Play(Sound& sound, int channel)
			{
				Mix_PlayChannel(static_cast<int>(channel), sound.GetData(), 0);
			}

			void StopChannel(int channel)
			{
				Mix_HaltChannel(static_cast<int>(channel));
			}

			void SetChannelVolume(int channel, int volume)
			{
				Mix_Volume(static_cast<int>(channel), volume);
			}

		};
	}

}