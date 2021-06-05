#pragma once

#include "UIElement.hpp"
#include "LayoutGroup.hpp"
#include "FlowLayout.hpp"
#include "Button.hpp"
#include "Dragable.hpp"
#include "Transforms.hpp"


namespace cage
{
	namespace ui
	{
		class ScrollPanel : public LayoutGroup
		{

			// The content of the panel
			class ContentPane : public UIElement
			{
			public:

				void Add(Child child) override
				{
					child->SetLocalMounting(MountPoint::TOP);
					child->SetParentMounting(MountPoint::TOP);
					UIElement::Add(child);
				}

				std::string GetName() override
				{
					return "Scroll Content Pane";
				}
			};

			// The entire scroll bar, not just the nested box
			class ScrollBar : public Clickable
			{
			public:
				ScrollBar() : Clickable(std::nullopt, std::nullopt)
				{
					m_bar = std::make_shared<cage::ui::Dragable>(std::nullopt, std::nullopt);
					m_bar->OnHover = [this]() { m_bar->SetColor({ 0.5, 0.5, 0.5, 1.0 }); };
					m_bar->OnUnHover = [this]() { m_bar->SetColor({ 0.3, 0.3, 0.3, 1.0 }); };
					m_bar->SetLocalMounting(MountPoint::TOP);
					m_bar->SetParentMounting(MountPoint::TOP);
					m_bar->SetColor({ 0.3, 0.3, 0.3, 1.0 });

					m_bar->OnDrag = [this](glm::vec2 dst)
					{
						dst += m_bar->GetPosition();
						dst.x = 0;
						dst.y = std::min(dst.y, GetSize().y - m_bar->GetSize().y);
						dst.y = std::max(dst.y, 0.f);

						m_targetPos = dst;
						m_bar->MoveTo(dst);
					};

					Add(m_bar);
					m_targetPos = m_bar->GetPosition();
				}
				std::shared_ptr<Dragable> m_bar;
				glm::vec2 m_targetPos;

				bool HandleEvent(Event& e) override
				{
					if (m_bar->HandleEvent(e))
						return true;
					if (Clickable::HandleEvent(e))
						return true;
					return false;
				}

				std::string GetName() override
				{
					return "Scrollbar";
				}

			private:

				bool onClick(int x, int y) override
				{
					if (hovering())
					{
						glm::vec4 mouse(x, y, 0.0f, 1.0f);
						mouse = glm::inverse(GetTransform()) * mouse;

						mouse.x = 0;
						mouse += glm::vec4(GetMountOffset(MountPoint::TOP, GetSize(), GetScale()), 0.0f, 0.0f);
						mouse -= glm::vec4(GetMountOffset(MountPoint::TOP, m_bar->GetSize(), m_bar->GetScale()), 0.f, 0.f);
						mouse.y = std::min(mouse.y, GetSize().y - m_bar->GetSize().y);
						mouse.y = std::max(mouse.y, 0.f);

						m_targetPos = mouse;
						m_bar->ScheduleTransform(std::make_unique<transforms::MoveTo>(m_targetPos, 0, 0.2, transforms::Interpolation::CUBIC));
						return true;
					}
					return false;
				}

				bool onRelease() override
				{
					return false;
				}
			};

		public:

			const int k_width = 20;

			std::string GetName() override
			{
				return "Scroll Panel";
			}

			ScrollPanel(glm::vec2 size) : 
				LayoutGroup(new FlowLayout()),
				m_scrollPanelHeight(size.y),
				m_selected(false)
			{
				m_scrollBar = std::make_shared<ScrollBar>();
				m_contentPane = std::make_shared<ContentPane>();
				m_contentPane->SetMasking(true);

				m_contentPane->Resize(size);

				m_scrollBar->SetColor({ 0.2, 0.2, 0.2, 1.0 });
				m_scrollBar->Resize({ k_width, size.y });
				LayoutGroup::SetColor({ 0.6, 0.6, 0.6, 0.3 });

				m_bar = m_scrollBar->m_bar;

				LayoutGroup::Add(m_contentPane);
				LayoutGroup::Add(m_scrollBar);
			}

			bool HandleEvent(Event& e) override
			{
				if (auto ce = std::get_if<MouseMotionEvent>(&e))
				{
					glm::vec4 mouse((float)ce->x, (float)ce->y, 0.0f, 1.0f);
					mouse = glm::inverse(GetTransform()) * mouse;
					mouse /= glm::vec4(GetSize(), 1.0f, 1.0f);
					bool inBounds = mouse.x >= -0.5 && mouse.x <= 0.5 && mouse.y >= -0.5 && mouse.y <= 0.5;

					if (inBounds)
					{
						if (!m_selected)
						{
							m_selected = true;
							return true;
						}
					}
					else
					{
						m_selected = false;
						return false;
					}
				}

				if (m_selected)
				{
					if (m_scrollBar->HandleEvent(e))
					{
						updateContentPos();
						return true;
					}
					if (m_content->HandleEvent(e))
						return true;
					if (auto se = std::get_if<ScrollEvent>(&e))
					{
						int scroll = se->amount;

						glm::vec2 newPos = m_scrollBar->m_targetPos - glm::vec2{ 0.0f, 50.0f * scroll };
						newPos.y = std::min(newPos.y, m_scrollPanelHeight - m_bar->GetSize().y);
						newPos.y = std::max(0.f, newPos.y);

						m_scrollBar->m_targetPos = newPos;
						m_bar->HaltCurrentTransform();
						m_bar->ScheduleTransform(std::make_unique<transforms::MoveTo>(newPos, 0, 0.2, transforms::Interpolation::CUBIC));
						updateContentPos();

						return true;
					}
				}
				return false;
			}

			void Add(Child child) override
			{
				m_contentPane->Add(child);
				m_content = child;
				m_bar->Resize({ k_width, std::min(m_scrollPanelHeight * m_scrollPanelHeight / m_content->GetScaledSize().y, m_scrollPanelHeight) });
			}

			void onTransform() override
			{
				m_scrollPanelHeight = GetScaledSize().y;
				//m_contentPane->Resize(GetSize());
				//m_scrollBar->Resize({ k_width, GetSize().y });;
			}

		private:

			void updateContentPos()
			{
				m_scrollProgress = m_scrollBar->m_targetPos.y / (m_scrollPanelHeight - m_bar->GetSize().y);
				if (std::isnan(m_scrollProgress))
					m_scrollProgress = 0.0;
				glm::vec2 pos = { 0.f, -m_content->GetScaledSize().y + m_scrollPanelHeight };
				pos *= m_scrollProgress;

				m_content->HaltCurrentTransform();
				m_content->ScheduleTransform(std::make_unique<transforms::MoveTo>(pos, 0, 0.2, transforms::Interpolation::CUBIC));
			}

			std::shared_ptr<ScrollBar> m_scrollBar;
			Child m_contentPane;
			Child m_content;
			Child m_bar;

			bool m_selected;

			// a normalized 0 - 1 indicator of how far the scroll progress is
			float m_scrollProgress;
			// how tall the box actually is, used in calculations to determine how to move the content and the sizing of the scroll bar 
			float m_scrollPanelHeight;
		};
	}
}