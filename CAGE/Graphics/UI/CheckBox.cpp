#include "CheckBox.hpp"

namespace cage
{
	namespace ui
	{
		CheckBox::CheckBoxButton::CheckBoxButton(TextureManager& texManager) : 
			Button(texManager.Get("checkbox.png"), std::nullopt, texManager.Get("check.png")), 
			m_selected(false)
		{
		}

		bool CheckBox::CheckBoxButton::onClick(int x, int y)
		{
			if (hovering())
			{
				if (m_selected)
				{
					SetActiveTexture(m_idleTexture.value());
					m_selected = false;
				}
				else
				{
					SetActiveTexture(m_clickTexture.value());
					m_selected = true;
				}
			}
			return hovering();
		}

		void CheckBox::CheckBoxButton::onHover()
		{

		}

		void CheckBox::CheckBoxButton::onUnHover()
		{

		}

		bool CheckBox::CheckBoxButton::onRelease()
		{
			return false;
		}

		CheckBox::CheckBox(TextureManager& texManager, const std::string& label, const Font& font)
			: LayoutGroup(new FlowLayout()), m_label(label)
		{
			//SetVisible(false);

			auto text = std::make_shared<Text>(font);
			text->SetText(label);

			m_button = std::make_shared<CheckBoxButton>(texManager);

			Add(m_button);
			Add(text);
		}

		bool CheckBox::HandleEvent(Event& e)
		{
			return m_button->HandleEvent(e);
		}

		bool CheckBox::Checked()
		{
			return m_button->m_selected;
		}
	}
}