#pragma once

#include <functional>

#include "Clickable.hpp"

namespace cage
{
	namespace ui
	{
		class Button : public Clickable
		{
		public:
			Button(std::optional<std::shared_ptr<Texture>> idleTexture, std::optional<std::shared_ptr<Texture>> hoverTexture, std::optional<std::shared_ptr<Texture>> clickTexture)
				: Clickable(idleTexture, hoverTexture), m_clickTexture(clickTexture)
			{
				m_clickStart = false;
			}

			std::function<void(void)> OnClick;

			
			bool HandleEvent(Event& e) override
			{
				if (Hoverable::HandleEvent(e))
					return true;
				if (auto ce = std::get_if<MouseClickEvent>(&e))
				{
					if (ce->wasRelease)
						return onRelease();
					else
						return onClick();
				}
				return false;
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

			virtual bool onClick() override
			{
				if (hovering())
				{
					m_clickStart = true;
					SetActiveTexture(m_clickTexture.value_or(m_idleTexture.value_or(nullptr)));
					return true;
				}
				return false;
			}

			virtual bool onRelease() override
			{
				if (hovering() && m_clickStart)
				{
					if (OnClick)
						OnClick();
					m_clickStart = false;
					SetActiveTexture(m_hoverTexture.value_or(m_idleTexture.value_or(nullptr)));
					return true;
				}
				SetActiveTexture(m_idleTexture.value_or(nullptr));
				m_clickStart = false;
				return false;
			}

		private:

			bool m_clickStart;

		};
	}
}