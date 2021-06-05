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

		std::string LayoutGroup::GetName()
		{
			return "Layout Group (" + m_layout->GetName() + ")";
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

		void LayoutGroup::Remove(std::shared_ptr<UIElement> element)
		{
			UIElement::Remove(element);
			m_layoutChildren.erase(std::remove(m_layoutChildren.begin(), m_layoutChildren.end(), element));
		}

		void LayoutGroup::AddAbsolute(std::shared_ptr<UIElement> element)
		{
			UIElement::Add(element);
		}

		std::vector<std::shared_ptr<UIElement>> LayoutGroup::GetLayoutChildren()
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