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
			FlowLayout(padding, Orientation::HORIZONTAL, leftJustify)
		{

		}

		FlowLayout::FlowLayout(glm::vec2 padding, Orientation orientation, bool leftJustify) :
			m_padding(padding), m_orientation(orientation), m_leftJustify(leftJustify)
		{

		}


		std::string FlowLayout::GetName()
		{
			return "Flow";
		}


		void FlowLayout::Update()
		{
			if (m_orientation == Orientation::HORIZONTAL)
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
			else
			{
				glm::vec2 pos = { 0.f, m_padding.y };

				float maxWidth = 0.f;

				for (auto v : m_container->GetLayoutChildren())
				{
					v->SetParentMounting(MountPoint::TOP);
					v->SetLocalMounting(MountPoint::TOP);

					v->MoveTo(pos);
					pos += glm::vec2{ 0.0f, v->GetScaledSize().y + m_padding.y};
					maxWidth = glm::max(maxWidth, v->GetScaledSize().x);
				}
				auto size = glm::vec2{ maxWidth + 2.f * m_padding.x, glm::max(m_container->GetScaledSize().y, pos.y) };
				m_container->Resize(size);
			}
		}
	}
}