#include "GridLayout.hpp"
#include "LayoutGroup.hpp"

namespace cage
{
	namespace ui
	{
		GridLayout::GridLayout(glm::vec2 padding, int cols) :
			m_padding(padding), m_cols(cols)
		{
		}

		void GridLayout::Update()
		{
			glm::vec2 maxSize = { 0.f, 0.f };
			for (auto child : m_container->GetLayoutChildren())
			{
				child->SetLocalMounting(MountPoint::CENTER);
				child->SetParentMounting(MountPoint::TOP_LEFT);

				maxSize = glm::max(maxSize, child->GetScaledSize());
			}
			maxSize += m_padding;

			int x = 0;
			int y = 0;

			auto children = m_container->GetLayoutChildren();
			for (int i = 0; i < children.size(); i++)
			{
				x = i % m_cols;
				y = i / m_cols;
				// pos = { x, y } * maxSize
				auto position = maxSize * (glm::vec2{ x, y } + glm::vec2{ 0.5f, 0.5f });
				children[i]->MoveTo(position);
			}
			glm::vec2 newSize = glm::max(m_container->GetScaledSize(), glm::vec2{ maxSize.x * (m_cols), maxSize.y * (y + 1) });
			m_container->Resize(newSize);
		}
	}
}