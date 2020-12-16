#pragma once

#include <string>

// Provides facilities for some very platform specific features
namespace cage
{
	namespace platform
	{
		void Init();
		std::string GetUsername();
	}
}