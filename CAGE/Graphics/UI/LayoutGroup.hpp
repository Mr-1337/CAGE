#pragma once

#include "Layout.hpp"

namespace cage
{
	namespace ui
	{
		class LayoutGroup : public UIElement
		{
		public:
			LayoutGroup(const Layout& layout);
			void Add(std::shared_ptr<UIElement> element) override;
		private:
			const Layout& m_layout;
		};
	}
}