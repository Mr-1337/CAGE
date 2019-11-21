#include "CheckBox.hpp"
#include <SDL2/SDL_image.h>

namespace cage
{
	namespace ui
	{
		CheckBox::CheckBoxButton::CheckBoxButton()
			: Button(std::make_shared<Texture>(IMG_Load("Assets/checkbox.png")), std::nullopt, std::make_shared<Texture>(IMG_Load("Assets/check.png"))), m_selected(false)
		{
		}

		bool CheckBox::CheckBoxButton::onClick()
		{
			if (m_hovering)
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
			return m_hovering;
		}

		void CheckBox::CheckBoxButton::onHover()
		{

		}

		void CheckBox::CheckBoxButton::onUnHover()
		{

		}

		CheckBox::CheckBox(const std::string& label, TTF_Font* font)
			: UIElement(false), m_label(label), m_font(font)
		{
			std::shared_ptr<UIElement> t = std::make_shared<Text>(font);
			std::static_pointer_cast<Text>(t)->SetText(label);
			Add(t);
			t->SetMounting(MountPoint::TOP_LEFT);
			m_button = std::make_shared<CheckBoxButton>();
			m_button->SetMounting(MountPoint::TOP_LEFT);
			t->MoveTo({ m_button->GetSize().x * 1.5, 0.f });
			Add(m_button);
			float w = m_button->GetSize().x + t->GetSize().x;
			float h = glm::max(m_button->GetSize().y, t->GetSize().y);
			Resize({ w, h });
		}

		bool CheckBox::Checked()
		{
			return std::static_pointer_cast<CheckBoxButton>(m_button)->m_selected;
		}
	}
}