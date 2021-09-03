#include "Animation.hpp"
#include <iostream>

namespace cage
{
	namespace animation
	{
		Animation::Animation(std::vector<std::vector<Keyframe>> animationChannels, std::vector<Bone> skeleton, float duration) : m_currentTime(0.0f), m_duration(duration), m_playing(false)
		{
			m_channels = animationChannels;
			m_currentPose = skeleton;
		}

		Animation::~Animation()
		{

		}

		void Animation::Play()
		{
			m_playing = true;
			m_currentTime = 0.0f;
		}

		void Animation::Stop()
		{
			m_playing = false;
			m_currentTime = 0.0f;
		}

		void Animation::Seek(float timeStamp)
		{
			m_currentTime = std::clamp(timeStamp, 0.0f, m_duration);
		}

		void Animation::Update(float dt)
		{
			if (m_playing)
				m_currentTime += dt;
			if (m_currentTime > m_duration)
			{
				m_currentTime = 0.0f;
				std::cout << "Animation reset\n";
			}

			// Process each channel
			int i = 0;
			for (const auto& channel : m_channels)
			{
				// Find the first keyframe that the playhead is beyond
				auto keyframe1 = std::find_if(channel.crbegin(), channel.crend(), [this](const auto& k) { return k.second <= m_currentTime; });
				if (keyframe1 != channel.crbegin() && keyframe1 != channel.crend())
				{
					// We have a second keyframe to interpolate to, do it
					auto keyframe2 = keyframe1 - 1;
					m_currentPose[i] = interpolateBone(*keyframe1, *keyframe2);
				}

				i++;
			}
		}

		std::vector<Bone>& Animation::GetPose()
		{
			return m_currentPose;
		}

		Bone Animation::interpolateBone(const Keyframe& kf1, const Keyframe& kf2)
		{
			Bone interpolated = kf1.first;
			const auto& b1 = kf1.first;
			const auto& b2 = kf2.first;
			float t = (m_currentTime - kf1.second) / (kf2.second - kf1.second);
			//t = 0.0;
			if (t > 1.0f || t < 0.0f)
				__debugbreak();
			interpolated.m_Orientation = glm::slerp(b1.m_Orientation, b2.m_Orientation, t);
			interpolated.m_Position = glm::mix(b1.m_Position, b2.m_Position, t);

			interpolated.m_LocalTransform = glm::translate(glm::identity<glm::mat4>(), interpolated.m_Position) * glm::toMat4(interpolated.m_Orientation);

			return interpolated;
		}


	}
}