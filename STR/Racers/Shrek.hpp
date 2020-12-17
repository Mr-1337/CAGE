#pragma once

#include "Racer.hpp"

namespace str
{

	class Shrek : public Racer
	{
	public:

		std::vector<std::string> GetSongs() override;
	};


}