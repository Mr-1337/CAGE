#pragma once

#include "Layout.hpp"

namespace cage
{
	namespace ui
	{
		class FlowLayout : public Layout
		{
			glm::vec2 m_padding;
			bool m_leftJustify;
		public:
			FlowLayout(glm::vec2 padding, bool leftJustify = false);
			FlowLayout();
			void Update() override;
		};
	}
}