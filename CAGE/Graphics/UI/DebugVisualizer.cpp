#include <algorithm>
#include <functional>
#include <GLM/glm/gtx/string_cast.hpp>
#include "Button.hpp"
#include "DebugVisualizer.hpp"

namespace cage
{
	namespace ui
	{

		DebugVisualizer::DebugVisualizer(UIElement* rootNode) : ScrollPanel({ 600.0f, 800.0f }),
			m_root(rootNode), m_selected(nullptr), m_count(0)
		{
			SetLocalMounting(MountPoint::TOP_LEFT);
			SetParentMounting(MountPoint::TOP_LEFT);

			m_content = std::make_shared<LayoutGroup>(new FlowLayout({ 10.0, 15.0 }, FlowLayout::Orientation::VERTICAL, true));

			auto text = std::make_shared<Text>(*UIElement::s_DefaultFont);
			m_content->Add(text);
			text->SetText("Super Epic Debug Tool\u2122");
			text->Scale(1.2);

			updateDrawTree();
			std::cout << GetSize().x << std::endl;
			std::cout << GetSize().y << std::endl;
			m_content->SetColor(glm::vec4{ 0.7f, 0.0f, 0.7f, 0.5f });

			m_highlight = std::make_shared<UIElement>();
			m_highlight->SetColor(glm::vec4{ 0.7f, 0.0f, 0.7f, 0.5f });

			m_axes = std::make_shared<UIElement>();
			Child x = std::make_shared<UIElement>();
			x->Resize({ 20, 2 });
			x->SetColor({ 1.0, 0.0, 0.0, 1.0 });
			x->SetLocalMounting(MountPoint::CENTER_LEFT);

			Child y = std::make_shared<UIElement>();
			y->Resize({ 2, 20 });
			y->SetColor({ 0.0, 1.0, 0.0, 1.0 });
			y->SetLocalMounting(MountPoint::BOTTOM);

			m_axes->Add(x);
			m_axes->Add(y);

			m_cross = std::make_shared<UIElement>();

			Child line1 = std::make_shared<UIElement>();
			Child line2 = std::make_shared<UIElement>();
			line1->Resize({ 20, 2 });
			line2->Resize({ 20, 2 });
			line1->SetRotation(glm::radians(45.f));
			line2->SetRotation(glm::radians(-45.f));
			line1->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			line2->SetColor({ 1.0, 1.0, 1.0, 1.0 });
			m_cross->Add(line1);
			m_cross->Add(line2);

			m_highlight->Add(m_cross);
			m_highlight->Add(m_axes);
			m_highlight->IgnoreMasking(true);

			m_content->AddAbsolute(m_highlight);
			Add(m_content);
			//Resize(m_content->GetSize());
		}

		void DebugVisualizer::Update(float dt)
		{
			if (m_selected != nullptr)
			{
				m_highlight->Resize(m_selected->GetScaledSize());
				m_highlight->SetTransform(m_selected->GetTransform());

				m_axes->SetParentMounting(m_selected->GetLocalMounting());
				m_cross->SetParentMounting(m_selected->GetLocalMounting());
				m_cross->MoveTo(m_selected->GetPosition() - m_selected->GetParentMountOffset());
			}

			m_listRoot->Update();
			UIElement::Update(dt);
		}

		void DebugVisualizer::updateDrawTree()
		{
			auto topElement = std::make_shared<ElementList>(nullptr, this);
			m_listRoot = topElement.get();
			m_listRoot->SetLocalMounting(MountPoint::TOP_LEFT);
			m_listRoot->SetParentMounting(MountPoint::TOP_LEFT);
			m_content->Add(topElement);
			traverseStep(m_root, topElement);
			topElement->Update();
			m_content->LayoutGroup::Update();
		}

		// A single step of the tree traversal. Each step takes an item from the scene graph and adds an item to the list
		void DebugVisualizer::traverseStep(UIElement* node, std::shared_ptr<ElementList> listNode)
		{
			// Add item to the parent's list
			auto listItem = std::make_shared<ElementList>(node, this);
			listNode->Add(listItem);
			// Indent
			listItem->MoveTo(glm::vec2{ 20.0, 0.0 });
			listNode = listItem;

			// Add properties
			addElementProperties(listItem, node);

			// Get the children and do an inorder tree traversal to add all of them
			auto children = node->GetChildren();

			std::for_each(children.begin(), children.end(), [&](auto child) {
				// For each child, traverse their tree
				traverseStep(child.get(), listNode);
				m_listRoot->Update();
			});
		}

		void DebugVisualizer::addElementProperties(std::shared_ptr<ElementList> listNode, UIElement* node)
		{

			auto addProp = [listNode](const std::string& label, auto vector) {
				auto property = std::make_shared<Text>(*UIElement::s_DefaultFont);
				property->SetText(label + ": " + glm::to_string(vector));
				property->MoveTo({ 20.0, 0.0 });
				property->SetColor({ 0.6f, 0.6f, 0.6f, 1.0f });
				property->Scale(0.8);
				listNode->Add(property);
				listNode->m_PropTable[label] = property;
			};

			addProp("Size", node->GetSize());
			addProp("Position", node->GetPosition());
			addProp("Scale", node->GetScale());
			//addProp("Transform", node->GetTransform());
		}

		DebugVisualizer::ElementList::ElementList(UIElement* element, DebugVisualizer* dv) : LayoutGroup(new ListLayout()), m_dv(dv), m_element(element)
		{
			std::string text;
			//((ListLayout*)m_layout)->Toggle();

			if (element != nullptr)
			{
				text = element->GetName();
			}
			else
			{
				text = "Scene Graph";
			}

			id = m_dv->m_count;
			text.append(" - " + std::to_string(m_dv->m_count++));


			auto texture = UIElement::s_DefaultFont->Render(text, { 255, 255, 255, 255 });
			auto texture2 = UIElement::s_DefaultFont->Render(text, { 0, 255, 0, 255 });
			auto button = std::make_shared<Button>(texture, texture2, std::nullopt);
			button->OnClick = [this, button]()
			{
				((ListLayout*)m_layout)->Toggle();
				m_dv->m_listRoot->Update();
			};

			button->OnHover = [this, element]()
			{
				if (element != nullptr)
				{
					m_dv->m_selected = element;
				}
			};

			Add(button);
		}

		void DebugVisualizer::ElementList::Update()
		{
			for (auto c : GetChildren())
			{
				auto casted = dynamic_cast<ElementList*>(c.get());
				if (casted)
					casted->Update();
			}
			LayoutGroup::Update();

			auto updateProp = [this](const std::string& label, auto vector) {
				//m_PropTable[label]->SetText(label + ": " + glm::to_string(vector));
			};

			if (!m_PropTable.empty())
			{
				if (m_PropTable["Size"])
					updateProp("Size", m_element->GetSize());
				if (m_PropTable["Position"])
					updateProp("Position", m_element->GetPosition());
				if (m_PropTable["Scale"])
					updateProp("Scale", m_element->GetScale());
			}
			//m_dv->LayoutGroup::Compress();
		}
	}
}