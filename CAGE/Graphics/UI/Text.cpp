#include "Text.hpp"

namespace cage
{
	namespace ui
	{
		Text::Text(const Font& font) : m_font(font), m_color{ 255, 255, 255, 255 }
		{

		}

		void Text::SetText(const std::string& text)
		{
			m_text = text.empty() ? " " : text;
			SetActiveTexture(m_font.Render(m_text, m_color));
		}

		void Text::SetColor(SDL_Color color)
		{
			m_color = color;
		}
	}
}