#ifdef _WIN32
#include <Windows.h>
#endif

#include "Platform.hpp"

namespace cage
{
	namespace platform
	{
		// I can't capitalize the n here because it will conflict with a Windows #define. Thanks Windows!
#ifdef _WIN32
		std::string GetUsername()
		{
			const unsigned int MAX_SIZE = 64;
			char buffer[MAX_SIZE];
			DWORD size = 64;
			GetUserNameA(buffer, &size);
			std::string username = buffer;

			return username.substr(0, size);
		}

		void Init()
		{
			//SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}

#endif

	}

}