#pragma once

#include <SDL2/SDL_ttf.h>
#include "UIElement.hpp"

namespace cage
{
	namespace ui
	{
		class Text : public UIElement
		{
		public:
			Text(TTF_Font* font);
			void SetColor(SDL_Color color);
			void SetText(const std::string& text);
		private:
			std::string m_text;
			TTF_Font* m_font;
			SDL_Color m_color;
		};
	}
}