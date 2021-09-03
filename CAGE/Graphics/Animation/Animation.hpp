#pragma once

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "Bone.hpp"

namespace cage
{
	namespace animation
	{
		using Keyframe = std::pair<Bone, float>;

		// Represents a 3D animation that could be applied to a Mesh / skeleton
		class Animation
		{
			float m_currentTime;
			float m_duration;
			bool m_playing;

			std::vector<std::vector<Keyframe>> m_channels;
			std::vector<Bone> m_currentPose;

			Bone interpolateBone(const Keyframe& kf1, const Keyframe& kf2);

		public:
			Animation(std::vector<std::vector<Keyframe>> animationChannels, std::vector<Bone> skeleton, float duration);
			~Animation();

			void Play();
			void Stop();
			void Seek(float timeStamp);
			void Update(float dt);
			
			inline float GetDuration() { return m_duration; }

			std::vector<Bone>& GetPose();
		};
	}
}