#include "Clickable.hpp"

#pragma once

namespace cage
{
	namespace ui
	{
		class Dragable : public Clickable
		{
		public:
			Dragable(std::optional<std::shared_ptr<Texture>> idleTexture, std::optional<std::shared_ptr<Texture>> hoverTexture) : 
				Clickable(idleTexture, hoverTexture),
				m_dragging(false)
			{

			}

			std::function<void(glm::vec2)> OnDrag;

			bool HandleEvent(Event& e) override
			{
				if (Clickable::HandleEvent(e))
					return true;
				if (auto me = std::get_if<MouseMotionEvent>(&e))
				{
					if (m_dragging)
					{
						glm::vec4 mouse(me->dx, me->dy, 0.f, 0.0f);
						mouse = glm::inverse(GetTransform()) * mouse;
						if (OnDrag)
							OnDrag({ mouse.x, mouse.y });

						return true;
					}
				}
				return false;
			}

		private:

			bool m_dragging;

			void onTransform() override
			{

			}

			bool onClick(int x, int y) override
			{
				if (hovering())
				{
					m_dragging = true;
					return true;
				}
				return false;
			}

			bool onRelease() override
			{
				m_dragging = false;
				return false;
			}
		};
	}
}