#pragma once

#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtx/quaternion.hpp>
#include <string>

namespace cage
{
	namespace animation
	{
		class Bone
		{
		public:
			Bone() {};

			glm::mat4 m_LocalTransform, m_Offset;

			glm::vec3 m_Position;
			glm::quat m_Orientation;

			int m_Parent;
			std::string m_Name;
		};
	}
}