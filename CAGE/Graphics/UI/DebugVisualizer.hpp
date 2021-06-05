#pragma once

#include "LayoutGroup.hpp"
#include "GridLayout.hpp"
#include "FlowLayout.hpp"
#include "ListLayout.hpp"
#include "Text.hpp"
#include "ScrollPanel.hpp"

namespace cage
{
	namespace ui
	{
		/*
		* The DebugVisualizer provides helpful visual tools for diagnosing problems with UI rendering. 
		* It showcases each element's state as well as how they are being rendered
		* 
		* 
		*/
		class DebugVisualizer : public ScrollPanel
		{
			// Helper element for dropdown lists for each object in the UI tree list
			class ElementList : public LayoutGroup
			{
				DebugVisualizer* m_dv;
				UIElement* m_element;
				int id;
			public:
				ElementList(UIElement* element, DebugVisualizer* dv);
				void Update() override;

				std::unordered_map<std::string, std::shared_ptr<Text>> m_PropTable;
			};
			// Generates a list of all the elements
			void updateDrawTree();
			void traverseStep(UIElement* node, std::shared_ptr<ElementList> listNode);

			ElementList* m_listRoot;
			UIElement* m_root, *m_selected;
			Child m_highlight, m_axes, m_cross;
			std::shared_ptr<LayoutGroup> m_content;
			int m_count;

			void addElementProperties(std::shared_ptr<ElementList> listItem, UIElement* node);

		public:
			DebugVisualizer(UIElement* rootNode);
			void Update(float dt) override;
		};
	}
}