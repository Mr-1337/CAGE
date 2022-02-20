#include "Texture.hpp"
#include <iostream>

namespace cage
{

	Texture* Texture::s_MissingTexture;

	Texture::Texture(SDL_Surface* textureData, bool keepLocalCopy) : 
		m_textureUnit(0),
		m_size(textureData->w, textureData->h),
		m_surface(textureData)
	{
		glGenTextures(1, &m_id);
		Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		uploadTexData(m_surface);

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void Texture::uploadTexData(SDL_Surface* surface)
	{
		auto glFormat = getGLPixelFormat(surface->format);
		auto [w, h] = GetSize();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, glFormat, GL_UNSIGNED_BYTE, m_surface->pixels);
	}

	GLenum Texture::getGLPixelFormat(SDL_PixelFormat* sdlFormat)
	{
		int numColors = sdlFormat->BytesPerPixel;
		if (numColors == 4)
		{
			if (sdlFormat->Rmask == 0x000000FF)
				return GL_RGBA;
			else
				return GL_BGRA;
		}
		else if (numColors == 3)
		{
			if (sdlFormat->Rmask == 0x000000FF)
				return GL_RGB;
			else
				return GL_BGR;
		}
		return GL_RGBA;
	}
}