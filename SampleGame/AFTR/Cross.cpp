#include "Cross.hpp"

namespace aftr
{
	Cross::Cross()
	{
		m_connections = (uint8_t)Directions::UP | (uint8_t)Directions::DOWN | (uint8_t)Directions::NORTH | (uint8_t)Directions::SOUTH | (uint8_t)Directions::EAST | (uint8_t)Directions::WEST;
	}

	Directions Cross::GetOrientation()
	{
		return Directions::UP;
	}

	void Cross::Update()
	{

	}
}