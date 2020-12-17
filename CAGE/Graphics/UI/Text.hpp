#pragma once

#include "../Font/Font.hpp"
#include "UIElement.hpp"

namespace cage
{
	namespace ui
	{
		class Text : public UIElement
		{
		public:
			Text(const Font& font);
			void SetColor(glm::vec4 color) override;
			void SetText(const std::string& text);
		private:
			std::string m_text;
			const Font& m_font;
			SDL_Color m_color;
		};
	}
}