#pragma once
#include "UIElement.hpp"

namespace cage
{
	namespace ui
	{
		/* 
		 * Represents a layout configuration for a LayoutGroup. A LayoutGroup will pass an element to a Layout and the Layout will determine where the element should be placed
		 * An implementing layout simply needs to have a way to provide
		 *
		 *
		 *
		 */
		class Layout
		{
		public:
			virtual glm::vec2 GetPosition(const UIElement& element) const = 0;
		protected:
		};
	}
}