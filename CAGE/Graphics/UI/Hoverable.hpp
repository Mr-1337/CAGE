#pragma once

#include "UIElement.hpp"

#include "../../IO/Events/EventListener.hpp"

namespace cage
{
	namespace ui
	{
		class Hoverable : public UIElement
		{
		public:
			Hoverable(std::optional<std::shared_ptr<Texture>> idleTexture, std::optional<std::shared_ptr<Texture>> hoverTexture) : 
				m_hovering(false),
				m_idleTexture(idleTexture), m_hoverTexture(hoverTexture)
			{
				m_lastX = 0;
				m_lastY = 0;
				SetActiveTexture(idleTexture.value());
			}

			virtual inline bool HandleEvent(Event& e)
			{
				if (auto mm = std::get_if<MouseMotionEvent>(&e))
				{
					m_lastX = mm->x;
					m_lastY = mm->y;
					if (inBounds(mm->x, mm->y))
					{
						onHover();
						m_hovering = true;
						return true;
					}
					onUnHover();
					m_hovering = false;
					return false;
				}
				return false;
			}

		protected:

			bool m_hovering;
			std::optional<std::shared_ptr<Texture>> m_idleTexture, m_hoverTexture;

			virtual void onHover()
			{
				SetActiveTexture(m_hoverTexture.value_or(m_idleTexture.value_or(nullptr)));
			}

			virtual void onUnHover()
			{
				SetActiveTexture(m_idleTexture.value_or(nullptr));
			}

			inline bool inBounds(int x, int y)
			{
				auto mat = glm::inverse(GetTransform());
				glm::vec4 temp{ (float)x, (float)y, 0.f, 1.0f };
				temp = mat * temp;
				temp /= glm::vec4{ GetSize(), 0.f, 1.0f };

				return (temp.x >= -0.5 && temp.x <= 0.5) && (temp.y >= -0.5 && temp.y <= 0.5);
			}

		private:

			int m_lastX, m_lastY;

			void onTransform() override
			{
				Event e = MouseMotionEvent(m_lastX, m_lastY, 0, 0);
				HandleEvent(e);
			}

		};
	}
}