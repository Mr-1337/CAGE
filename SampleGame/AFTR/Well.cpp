#include "Well.hpp"

typedef char uint8_t;

namespace aftr
{
	Well::Well()
	{
		switch (Directions::UP)
		{
		case Directions::UP:
		case Directions::DOWN:
			m_connections |= (uint8_t)Directions::UP | (uint8_t)Directions::DOWN;
			break;
		case Directions::NORTH:
		case Directions::SOUTH:
			m_connections |= (uint8_t)Directions::NORTH | (uint8_t)Directions::SOUTH;
			break;
		case Directions::EAST:
		case Directions::WEST:
			m_connections |= (uint8_t)Directions::EAST | (uint8_t)Directions::WEST;
			break;
		}
	}
	bool Well::GetState()
	{
		return true;
	}
	void Well::Update()
	{
		//State = true;
	}
	Directions Well::GetOrientation()
	{
		return Directions::UP;
	}
}
