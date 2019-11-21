#include "AudioSource.hpp"
#include <iostream>

namespace cage
{

	AudioSource::AudioSource() : m_channel(-1)
	{

	}

	void AudioSource::UpdateListener(const Listener& listener)
	{
		uint8_t distance;
		Sint16 angle;

		glm::vec3 lFront = listener.GetFront(), lPos = listener.GetPosition(), lRight = listener.GetRight();
		lFront.y = 0;
		lFront = glm::normalize(lFront);

		distance = (uint8_t) glm::min(glm::distance(lPos, m_position) * 5, 250.f);

		float dot = (glm::dot(lFront, m_position - lPos));
		float dot2 = glm::dot(lRight, m_position - lPos);

		float mag = glm::length(m_position - lPos);

		float theta = glm::degrees(glm::acos(dot / mag));

		angle = (Sint16)theta * ((dot2 >= 0) ? 1 : -1) + 360;

		if (m_channel == 1)
			std::cout << angle << '\n';

		Mix_SetPosition(m_channel, angle, distance);
	}

	void AudioSource::Play(Mix_Chunk* audio)
	{
		m_channel = Mix_PlayChannel(m_channel, audio, -1);
	}

	void AudioSource::Stop()
	{
		Mix_HaltChannel(m_channel);
	}

	AudioSource& AudioSource::MoveTo(glm::vec3 position)
	{
		m_position = position;
		return *this;
	}

}