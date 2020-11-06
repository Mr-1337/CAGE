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
		protected:
			virtual bool onClick() = 0;
			virtual bool onRelease() = 0;
		};
	}
}