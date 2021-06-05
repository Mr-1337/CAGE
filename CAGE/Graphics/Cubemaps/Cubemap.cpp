#include <SDL2/SDL_image.h>
#include <array>
#include "Cubemap.hpp"

namespace cage
{
	Cubemap::Cubemap(std::filesystem::path directory)
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

		std::array<SDL_Surface*, 6> images;
		std::array<std::string, 6> names = { "right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png" };

		for (int i = 0; i < images.size(); i++)
		{
			images[i] = IMG_Load((directory/names[i]).string().c_str());
		}

		for (unsigned int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, images[i]->w, images[i]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->pixels);
			SDL_FreeSurface(images[i]);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	Cubemap::~Cubemap()
	{
		glDeleteTextures(1, &m_id);
	}

	void Cubemap::Bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	}
}