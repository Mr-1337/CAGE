#pragma once

#include <Graphics/UI/UIElement.hpp>

namespace ub
{
	class ProgressBar : public cage::ui::UIElement
	{
		Child m_fgBar;
	public:
		ProgressBar(glm::vec2 size)
		{
			Resize(size);
			m_fgBar = std::make_shared<cage::ui::UIElement>();
			m_fgBar->Resize(size - glm::vec2{ 8.0, 8.0 });
			m_fgBar->SetParentMounting(cage::ui::MountPoint::CENTER_LEFT);
			m_fgBar->SetLocalMounting(cage::ui::MountPoint::CENTER_LEFT);
			m_fgBar->MoveTo({ 4.0, 0.0 });

			SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
			SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);

			UIElement::SetColor({ 0.7, 0.1, 0.1, 1.0 });
			m_fgBar->SetColor({ 0.0, 1.0, 0.0, 1.0 });
			Add(m_fgBar);
		}

		void SetColor(glm::vec4 color) override
		{
			m_fgBar->SetColor(color);
		}

		void SetProgress(float progress)
		{
			m_fgBar->SetScale({ progress, 1.0 });
		}
	};
}