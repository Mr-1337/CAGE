#pragma once

#include <SDL2/SDL_mixer.h>
#include <GLM/glm/glm.hpp>

namespace cage
{
	// Represents your "ears" in the 3D environment, used to calculate how audio should be attenuated / panned
	class Listener
	{
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_right;
	public:
		glm::vec3 GetFront() const;
		glm::vec3 GetPosition() const;
		glm::vec3 GetRight() const;

		Listener& MoveTo(glm::vec3 position);
		Listener& LookAt(glm::vec3 direction, glm::vec3 right);

	};
}