#include "Texture.hpp"

namespace cage
{
	class TextureArray
	{
		std::pair<int, int> m_size;
		int m_mipLevels;
		unsigned int m_id;
	public:
		TextureArray(SDL_Surface* textureData, bool keepLocalCopy = false);
	};
}