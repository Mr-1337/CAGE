#pragma once

#include "UIElement.hpp"

#include <optional>
#include <functional>

namespace cage
{
	namespace ui
	{
		class Hoverable : public UIElement
		{
		public:
			Hoverable(std::optional<std::shared_ptr<Texture>> idleTexture, std::optional<std::shared_ptr<Texture>> hoverTexture) : 
				m_idleTexture(idleTexture), m_hoverTexture(hoverTexture),
				m_hovering(false)
			{
				m_lastX = 0;
				m_lastY = 0;
				if (idleTexture.has_value())
					SetActiveTexture(*idleTexture);
			}

			virtual bool HandleEvent(Event& e)
			{
				if (auto mm = std::get_if<MouseMotionEvent>(&e))
				{
					m_lastX = mm->x;
					m_lastY = mm->y;
					if (inBounds(mm->x, mm->y))
					{
						if (OnHover && !hovering())
							OnHover();
						onHover();
						m_hovering = true;
					}
					else
					{
						if (m_hovering && OnUnHover)
							OnUnHover();
						onUnHover();
						m_hovering = false;
					}
				}
				return false;
			}

			std::function<void(void)> OnHover, OnUnHover;

		protected:

			std::optional<std::shared_ptr<Texture>> m_idleTexture, m_hoverTexture;

			bool hovering()
			{
				return m_hovering;
			}

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
				temp /= glm::vec4{ GetSize(), 1.0f, 1.0f };

				return (temp.x >= -0.5 && temp.x <= 0.5) && (temp.y >= -0.5 && temp.y <= 0.5);
			}

			void onTransform() override
			{
				//Event e = MouseMotionEvent(m_lastX, m_lastY, 0, 0);
				//HandleEvent(e);
			}

		private:

			int m_lastX, m_lastY;

			bool m_hovering;


		};
	}
}