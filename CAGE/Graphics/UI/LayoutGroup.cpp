#include "LayoutGroup.hpp"

namespace cage
{
	namespace ui
	{
		LayoutGroup::LayoutGroup(const Layout& layout) :
			m_layout(layout)
		{

		}

		void LayoutGroup::Add(std::shared_ptr<UIElement> element)
		{
			UIElement::Add(element);

			
		}
	}
}