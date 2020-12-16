#pragma once

#include "Hoverable.hpp"

namespace cage
{
	namespace ui
	{
		class Clickable : public Hoverable
		{
		public:
			Clickable(std::optional<std::shared_ptr<Texture>> idleTexture, std::optional<std::shared_ptr<Texture>> hoverTexture) :
				Hoverable(idleTexture, hoverTexture)
			{

			}

			bool HandleEvent(Event& e) override
			{
				if (Hoverable::HandleEvent(e))
					return true;
				if (auto ce = std::get_if<MouseClickEvent>(&e))
				{
					if (ce->wasRelease)
						return onRelease();
					else
						return onClick(ce->x, ce->y);
				}
				return false;
			}
		protected:
			virtual bool onClick(int x, int y) = 0;
			virtual bool onRelease() = 0;
		};
	}
}