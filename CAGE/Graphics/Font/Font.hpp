#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>

#include "../Textures/Texture.hpp"

namespace cage
{
	class Font
	{
		TTF_Font* m_font;
	public:
		Font(const std::string& fontName, int ptSize);
		std::shared_ptr<cage::Texture> Render(const std::string& text, SDL_Color color) const;
		~Font();
	};
}