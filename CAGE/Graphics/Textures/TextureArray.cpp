#include "TextureArray.hpp"

namespace cage
{
	TextureArray::TextureArray(SDL_Surface* textureData, bool keepLocalCopy) : m_id(0), m_size({ 0, 0 }), m_mipLevels(4)
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
	}
}