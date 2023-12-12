#include "Font.hpp"
#include <stdexcept>

namespace cage
{
	Font::Font(const std::string& fontName, int ptSize)
	{
		m_font = TTF_OpenFont(fontName.c_str(), ptSize);
		if (m_font == nullptr)
			throw std::runtime_error("Font file " + fontName + " not found");
	}

	Font::~Font()
	{
		TTF_CloseFont(m_font);
	}

	std::shared_ptr<cage::Texture> Font::Render(const std::string& text, SDL_Color color) const
	{
		return std::make_shared<cage::Texture>(TTF_RenderUTF8_Blended(m_font, text.c_str(), color));
	}

	std::shared_ptr<cage::graphics::VkTexture> Font::RenderVk(const std::string& text, SDL_Color color) const
	{
		return std::make_shared<cage::graphics::VkTexture>(TTF_RenderUTF8_Blended(m_font, text.c_str(), color));
	}


}