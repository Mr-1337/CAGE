#pragma once

#include <string>
#include "../World/World.hpp"
#include "../../CAGE/Graphics/UI/ScrollPanel.hpp"

namespace ub
{
	class EditorPanel : public cage::ui::ScrollPanel
	{
	public:
		EditorPanel(glm::vec2 size, World* world, const std::string& name) :
			ScrollPanel(size),
			m_world(world),
			m_name(name)
		{
			m_selectionHighlight = std::make_shared<cage::ui::UIElement>();
			m_selectionHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);

			m_hoverHighlight = std::make_shared<cage::ui::UIElement>();
			m_hoverHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);
		}

		std::string GetPanelName()
		{
			return m_name;
		};

	private:
		const std::string m_name;
	protected:
		World* m_world;
		std::shared_ptr<cage::ui::UIElement> m_selectionHighlight, m_hoverHighlight;
		std::shared_ptr<cage::ui::LayoutGroup> m_content;
	};
}