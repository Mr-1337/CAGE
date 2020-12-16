#pragma once

#include "Text.hpp"

namespace cage
{
	namespace ui
	{
		class TextField : public UIElement
		{
		public:
			TextField(const Font& font) :
				m_selected(false)
			{
				m_backBox = std::make_shared<UIElement>();
				m_backBox->SetColor({ 255, 255, 255, 255 });
				m_text = std::make_shared<Text>(font);
				Add(m_backBox);
				Add(m_text);
				Resize({ 50.f, 50.f });
				m_text->SetColor({ 0, 255, 0, 255 });
			}

			bool HandleEvent(Event& e) override
			{
				auto inBounds = [this](int x, int y)
				{
					glm::vec4 mouse(x, y, 0.0f, 1.0f);
					mouse = glm::inverse(m_backBox->GetTransform()) * mouse;
					mouse /= glm::vec4(m_backBox->GetSize(), 1.0f, 1.0f);
					return mouse.x >= -0.5f && mouse.x <= 0.5f && mouse.y >= -0.5f && mouse.y <= 0.5f;
				};

				if (auto ce = std::get_if<MouseClickEvent>(&e))
				{
					m_selected = inBounds(ce->x, ce->y);
					return true;
				}
				else if (auto te = std::get_if<TextEvent>(&e))
				{
					if (m_selected)
					{
						m_textString += te->text;
						m_text->SetText(m_textString);
						Resize(m_text->GetSize());
						return true;
					}
				}
				else if (auto kd = std::get_if<KeyDownEvent>(&e))
				{
					if (m_selected && !m_textString.empty() && kd->keyCode == SDLK_BACKSPACE)
					{
						m_textString.pop_back();
						m_text->SetText(m_textString);
						Resize(m_text->GetSize());
						return true;
					}
				}
				return false;
			}

		private:

			void onTransform() override
			{
				m_backBox->Resize(GetSize());
			}

			bool m_selected;
			std::string m_textString;
			Child m_backBox;
			std::shared_ptr<Text> m_text;
		};
	}
}