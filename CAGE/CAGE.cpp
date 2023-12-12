// CAGE.cpp : Defines the entry point for the application.

#include "CAGE.hpp"

namespace cage
{
	Version_t::operator unsigned int() const
	{
		return (major << 16) | (minor << 8) | patch;
	}

}