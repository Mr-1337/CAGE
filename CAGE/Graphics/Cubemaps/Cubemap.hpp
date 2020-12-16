#pragma once

#include <Glad/glad/glad.h>
#include <filesystem>

namespace cage
{
	class Cubemap
	{
		unsigned int m_id;
	public:
		Cubemap(std::filesystem::path directory);

		void Bind();

		~Cubemap();
	};
}