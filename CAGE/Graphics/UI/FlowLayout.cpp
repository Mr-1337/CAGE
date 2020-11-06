#include "LayoutGroup.hpp"
#include "FlowLayout.hpp"

namespace cage
{
	namespace ui
	{

		FlowLayout::FlowLayout() : FlowLayout({ 0.f, 0.f })
		{

		}

		FlowLayout::FlowLayout(glm::vec2 padding, bool leftJustify) :
			m_padding(padding), m_leftJustify(leftJustify)
		{

		}

		void FlowLayout::Update()
		{
			glm::vec2 pos = { 0.f, 0.f };
			if (!m_leftJustify)
				pos.x = m_padding.x;

			float maxHeight = 0.f;

			for (auto v : m_container->GetLayoutChildren())
			{
				v->SetParentMounting(MountPoint::CENTER_LEFT);
				v->SetLocalMounting(MountPoint::CENTER_LEFT);

				v->MoveTo(pos);
				pos += glm::vec2{ v->GetScaledSize().x + m_padding.x, 0.f };
				maxHeight = glm::max(maxHeight, v->GetScaledSize().y);
			}
			auto size = glm::vec2{ glm::max(m_container->GetScaledSize().x, pos.x), maxHeight + 2.f * m_padding.y };
			m_container->Resize(size);
		}
	}
}