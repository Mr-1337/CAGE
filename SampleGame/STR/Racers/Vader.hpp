#pragma once

#include "Racer.hpp"

namespace str
{

	class Vader : public Racer
	{
	public:

		std::vector<std::string> GetSongs() override;
	};

}