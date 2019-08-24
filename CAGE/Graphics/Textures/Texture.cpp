#include "Texture.hpp"

namespace cage
{
	Texture::Texture(SDL_Surface& textureData, bool keepLocalCopy) : 
		m_width(textureData.w), 
		m_height(textureData.h),
		m_pixelData(textureData.pixels),
		m_textureUnit(0)
	{
		glGenTextures(1, &m_id);

		Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		uploadTexData(textureData);

		glGenerateMipmap(GL_TEXTURE_2D);

		if (!keepLocalCopy)
		{
			SDL_FreeSurface(&textureData);
			m_pixelData = nullptr;
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void Texture::uploadTexData(SDL_Surface& surface)
	{
		auto glFormat = getGLPixelFormat(surface.format);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, glFormat, GL_UNSIGNED_BYTE, m_pixelData);
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