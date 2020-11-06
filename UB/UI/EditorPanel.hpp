#pragma once

#include <string>
#include "../World.hpp"
#include "../../CAGE/Graphics/UI/LayoutGroup.hpp"

namespace ub
{
	class EditorPanel : public cage::ui::LayoutGroup
	{
	public:
		EditorPanel(cage::ui::Layout* layout, World* world, const std::string& name) :
			LayoutGroup(layout),
			m_world(world),
			m_name(name)
		{
			m_selectionHighlight = std::make_shared<cage::ui::UIElement>();
			m_selectionHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);

			m_hoverHighlight = std::make_shared<cage::ui::UIElement>();
			m_hoverHighlight->SetParentMounting(cage::ui::MountPoint::TOP_LEFT);

			AddAbsolute(m_selectionHighlight);
			AddAbsolute(m_hoverHighlight);
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
	};
}