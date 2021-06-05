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

			enum class Orientation
			{
				HORIZONTAL,
				VERTICAL
			};

			FlowLayout(glm::vec2 padding, Orientation orientation, bool leftJustify);
			FlowLayout(glm::vec2 padding, bool leftJustify = false);
			FlowLayout();
			void Update() override;


			std::string GetName() override;

		private:
			Orientation m_orientation;
		};
	}
}