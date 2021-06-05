#pragma once

#include "Layout.hpp"

namespace cage
{
	namespace ui
	{
		class ListLayout : public Layout
		{
		public:

			ListLayout();
			ListLayout(glm::vec2 padding);
			void Update() override;

			std::string GetName() override;
			void Toggle();


		private:
			bool m_collapsed;
			glm::vec2 m_padding;
		};
	}
}