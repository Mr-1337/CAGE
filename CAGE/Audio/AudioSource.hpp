#pragma once

#include <SDL2/SDL_mixer.h>
#include <GLM/glm/glm.hpp>

#include "Listener.hpp"

namespace cage
{

	// Represents a source of audio in 3D space
	class AudioSource
	{
		glm::vec3 m_position;
		int m_channel;
	public:

		AudioSource();

		void UpdateListener(const Listener& listener);
		void Play(Mix_Chunk* audio);
		void Stop();

		AudioSource& MoveTo(glm::vec3 position);
	};

}