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
			void Add(Child element) override;
			void Remove(Child element) override;
			void AddAbsolute(Child element);
			virtual void Update();
			void Compress();

			std::string GetName() override;

			std::vector<Child> GetLayoutChildren();
		protected:
			Layout* m_layout;
			std::vector<Child> m_layoutChildren;
		};
	}
}