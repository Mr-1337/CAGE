#include "Text.hpp"

namespace cage
{
	namespace ui
	{
		Text::Text(TTF_Font* font) : m_font(font), m_color{ 255, 255, 255, 255}
		{

		}

		void Text::SetText(const std::string& text)
		{
			m_text = text.empty() ? " " : text;
			SetActiveTexture(std::make_shared<Texture>(TTF_RenderText_Blended(m_font, m_text.c_str(), m_color)));
		}

		void Text::SetColor(SDL_Color color)
		{
			m_color = color;
		}
	}
}