#pragma once
#include "Layout.hpp"

namespace cage
{
	namespace ui
	{
		class GridLayout : public Layout
		{
			glm::vec2 m_padding;
			int m_cols;
		public:
			GridLayout(glm::vec2 padding, int cols);
			void Update() override;
		};
	}
}