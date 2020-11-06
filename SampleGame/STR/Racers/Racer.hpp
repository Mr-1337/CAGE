#pragma once

#include <string>
#include <vector>
#include <random>


namespace str
{
	/*
	 * Contains all of the info about a particular racer
	 * 
	 *
	 */
	class Racer
	{

	public:

		virtual std::vector<std::string> GetSongs() = 0;

	private:

	};

}