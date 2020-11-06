#include <Windows.h>
#include "Platform.hpp"

namespace cage::platform
{
	// I can't capitalize the n here because it will conflict with a Windows #define. Thanks Windows!
	std::string GetUsername()
	{
		const unsigned int MAX_SIZE = 64;
		char buffer[MAX_SIZE];
		DWORD size = 64;
		GetUserNameA(buffer, &size);
		std::string username = buffer;

		return username.substr(0, size);
	}
}