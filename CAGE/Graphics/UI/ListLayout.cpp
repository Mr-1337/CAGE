#include "LayoutGroup.hpp"
#include "ListLayout.hpp"

namespace cage
{
	namespace ui
	{

		ListLayout::ListLayout() : m_collapsed(false), m_padding({ 0.f, 0.f })
		{

		}

		ListLayout::ListLayout(glm::vec2 padding) : m_collapsed(false), m_padding(padding)
		{

		}

		void ListLayout::Toggle()
		{
			m_collapsed = !m_collapsed;
		}

		std::string ListLayout::GetName()
		{
			return "List";
		}

		void ListLayout::Update()
		{

			auto lc = m_container->GetLayoutChildren();

			// toggle visibility
			for (auto i = lc.begin() + 1; i != lc.end(); i++)
			{
				(*i)->SetVisible(!m_collapsed);
			}

			float pos = 0;
			float maxWidth = 0;

			// for each child, stack them vertically
			// only visible elements should contribute
			for (auto v : m_container->GetLayoutChildren())
			{
				v->SetParentMounting(MountPoint::TOP_LEFT);
				v->SetLocalMounting(MountPoint::TOP_LEFT);

				v->MoveTo({ v->GetPosition().x, pos });

				if (v->IsVisible())
					pos += v->GetScaledSize().y;

				maxWidth = glm::max(maxWidth, v->GetScaledSize().x + v->GetPosition().x);
			}

			glm::vec2 size;
			
			size = { maxWidth, pos };

			m_container->Resize(size);

		}
	}
}