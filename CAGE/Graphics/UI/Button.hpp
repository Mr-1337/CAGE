#pragma once

#include <functional>
#include <optional>

#include "Hoverable.hpp"

namespace cage
{
	namespace ui
	{
		class Button : public Hoverable
		{
		public:
			Button(std::optional<std::shared_ptr<Texture>> idleTexture, std::optional<std::shared_ptr<Texture>> hoverTexture, std::optional<std::shared_ptr<Texture>> clickTexture)
				: Hoverable(idleTexture, hoverTexture), m_clickTexture(clickTexture)
			{
				m_clickStart = false;
			}

			std::function<void(void)> OnClick;

			
			bool HandleEvent(Event& e) override
			{
				Hoverable::HandleEvent(e);
				if (auto ce = std::get_if<MouseClickEvent>(&e))
				{
					if (ce->wasRelease)
						return onRelease();
					else
						return onClick();
				}
			}
			

		protected:

			std::optional<std::shared_ptr<Texture>> m_clickTexture;

			virtual void onHover() override
			{
				if (!m_clickStart)
					SetActiveTexture(m_hoverTexture.value_or(m_idleTexture.value_or(nullptr)));
				//SDL_SetCursor(s_cursor);
			}

			virtual void onUnHover() override
			{
				if (!m_clickStart)
					SetActiveTexture(m_idleTexture.value_or(nullptr));
				//SDL_SetCursor(s_cursor2);
			}

			virtual bool onClick()
			{
				if (m_hovering)
				{
					m_clickStart = true;
					SetActiveTexture(m_clickTexture.value_or(m_idleTexture.value()));
					std::cout << "BUTTON PRESSED" << std::endl;
					return true;
				}
				return false;
			}

			virtual bool onRelease()
			{
				if (m_hovering && m_clickStart)
				{
					if (OnClick)
						OnClick();
					m_clickStart = false;
					SetActiveTexture(m_hoverTexture.value_or(m_idleTexture.value()));
					return true;
				}
				SetActiveTexture(m_idleTexture.value());
				m_clickStart = false;
				return false;
			}

		private:

			bool m_clickStart;

		};
	}
}