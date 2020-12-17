#pragma once

#include "Text.hpp"

namespace cage
{
	namespace ui
	{
		class TextField : public UIElement
		{
		public:
			TextField(const Font& font, int width) :
				m_selected(false),
				m_maxChars(width)
			{
				m_backBox = std::make_shared<UIElement>();
				m_backBox->SetColor({ 1.0, 1.0, 1.0, 1.0 });
				m_text = std::make_shared<Text>(font);
				Add(m_backBox);
				Add(m_text);
				std::string dummy;
				for (int i = 0; i < m_maxChars; i++)
					dummy.append("_");
				m_text->SetText(dummy);
				m_text->SetParentMounting(MountPoint::CENTER_LEFT);
				m_text->SetLocalMounting(MountPoint::CENTER_LEFT);
				Resize(m_text->GetSize());
				m_text->SetText("");
				m_text->SetColor({ 0.0, 1.0, 0.0, 1.0 });
			}

			TextField(const Font& font) : TextField(font, 20)
			{

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
					if (inBounds(ce->x, ce->y))
					{
						m_selected = true;
						return true;
					}
					else
					{
						if (m_selected)
						{
							m_selected = false;
							return true;
						}
						else
						{
							return false;
						}
					}
				}
				else if (auto he = std::get_if<MouseMotionEvent>(&e))
				{
					return m_selected;
				}
				else if (auto te = std::get_if<TextEvent>(&e))
				{
					if (m_selected && m_textString.length() < m_maxChars)
					{
						m_textString += te->text;
						m_text->SetText(m_textString);
						auto size = glm::max(GetSize(), m_text->GetSize());
						Resize(size);
						return true;
					}
				}
				else if (auto kd = std::get_if<KeyDownEvent>(&e))
				{
					if (m_selected && !m_textString.empty() && kd->keyCode == SDLK_BACKSPACE)
					{
						m_textString.pop_back();
						m_text->SetText(m_textString);
						return true;
					}
				}
				return false;
			}

			std::string GetText()
			{
				return m_textString;
			}

		private:

			void onTransform() override
			{
				m_backBox->Resize(GetSize());
			}

			bool m_selected;
			int m_maxChars;
			std::string m_textString;
			Child m_backBox;
			std::shared_ptr<Text> m_text;
		};
	}
}