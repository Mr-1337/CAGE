#include "Texture.hpp"
#include <iostream>

namespace cage
{
	Texture::Texture(SDL_Surface* textureData, bool keepLocalCopy) : 
		m_surface(textureData),
		m_textureUnit(0)
	{
		glGenTextures(1, &m_id);
		m_surface->refcount++;
		Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		uploadTexData(m_surface);

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture::~Texture()
	{
		int count = --m_surface->refcount;
		if (count == 1)
			SDL_FreeSurface(m_surface);	
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
		return GL_RED;
	}
}