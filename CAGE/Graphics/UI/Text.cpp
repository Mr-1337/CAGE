#include "Text.hpp"

namespace cage
{
	namespace ui
	{
		Text::Text(const Font& font) : m_font(font), m_color{ 255, 255, 255, 255 }
		{

		}

		Text::Text() : Text(*s_DefaultFont)
		{

		}

		void Text::SetText(const std::string& text)
		{
			m_text = text.empty() ? " " : text;
			SetActiveTexture(m_font.Render(m_text, m_color));
		}

		void Text::SetColor(glm::vec4 color)
		{
			color *= 255.0;
			m_color = { (Uint8)color.x, (Uint8)color.y, (Uint8)color.z, (Uint8)color.a };
			SetText(m_text);
		}
	}
}