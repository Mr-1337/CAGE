#include "Pipe.hpp"
#include <iostream>

namespace aftr
{
	Pipe::Pipe(Directions orientation) : m_orientation(orientation)
	{
		switch (orientation)
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
		default:
			std::cout << "INVALID PIPE STATE!!!" << std::endl;
		}
	}

	Directions Pipe::GetOrientation()
	{
		return m_orientation;
	}

	void Pipe::Update()
	{

	}
}