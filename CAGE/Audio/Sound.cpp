#include "Sound.hpp"

namespace cage
{
	namespace audio
	{
		Sound::Sound(const char* filename) : m_chunk(Mix_LoadWAV(filename))
		{
		}

		Sound::~Sound()
		{
			Mix_FreeChunk(m_chunk);
		}

		Mix_Chunk* Sound::GetData()
		{
			return m_chunk;
		}
	}
}