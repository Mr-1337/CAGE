#include "Listener.hpp"

namespace cage
{

	glm::vec3 Listener::GetFront() const
	{
		return m_front;
	}

	glm::vec3 Listener::GetPosition() const
	{
		return m_position;
	}

	glm::vec3 Listener::GetRight() const
	{
		return m_right;
	}

	Listener& Listener::MoveTo(glm::vec3 position)
	{
		m_position = position;
		return *this;
	}

	Listener& Listener::LookAt(glm::vec3 direction, glm::vec3 right)
	{
		m_front = direction;
		m_right = right;
		return *this;
	}

}