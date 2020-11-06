#include "LayoutGroup.hpp"

namespace cage
{
	namespace ui
	{
		LayoutGroup::LayoutGroup(Layout* layout) :
			m_layout(layout)
		{
			m_layout->SetContainer(this);
		}

		LayoutGroup::~LayoutGroup()
		{
			delete m_layout;
		}

		void LayoutGroup::Add(std::shared_ptr<UIElement> element)
		{
			UIElement::Add(element);
			m_layoutChildren.push_back(element);
			Update();
		}

		void LayoutGroup::AddAbsolute(LayoutGroup::Child element)
		{
			UIElement::Add(element);
		}

		std::vector<LayoutGroup::Child> LayoutGroup::GetLayoutChildren()
		{
			return m_layoutChildren;
		}

		void LayoutGroup::Update()
		{
			m_layout->Update();
		}

		void LayoutGroup::Compress()
		{
			Resize({ 0.f, 0.f });
			Update();
		}
	}
}