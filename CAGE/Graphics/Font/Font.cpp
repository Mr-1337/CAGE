#include "Font.hpp"

namespace cage
{
	Font::Font(const std::string& fontName, int ptSize)
	{
		m_font = TTF_OpenFont(fontName.c_str(), ptSize);
	}

	Font::~Font()
	{
		TTF_CloseFont(m_font);
	}

	std::shared_ptr<cage::Texture> Font::Render(const std::string& text, SDL_Color color) const
	{
		return std::make_shared<cage::Texture>(TTF_RenderUTF8_Blended(m_font, text.c_str(), color));

	}


}