#pragma once

#include "UIElement.hpp"
#include "Layout.hpp"

namespace cage
{
	namespace ui
	{
		class LayoutGroup : public UIElement
		{
		public:
			LayoutGroup(Layout* layout);
			~LayoutGroup();
			void Add(std::shared_ptr<UIElement> element) override;
			void AddAbsolute(Child element);
			void Update();
			void Compress();

			std::vector<Child> GetLayoutChildren();
		private:
			Layout* m_layout;
			std::vector<Child> m_layoutChildren;
		};
	}
}