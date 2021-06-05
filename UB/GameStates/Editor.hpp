#pragma once

#include "../../CAGE/Core/GameState.hpp"
#include "../../CAGE/Graphics/UI/UIElement.hpp"
#include "../../CAGE/Graphics/UI/Button.hpp"
#include "../../CAGE/Graphics/UI/CheckBox.hpp"
#include "../World.hpp"
#include "../UI/TilePanel.hpp"
#include "../UI/StructurePanel.hpp"
#include "../UI/ScriptPanel.hpp"
#include <Graphics/UI/DebugVisualizer.hpp>


namespace ub
{
	class Editor : public cage::GameState
	{
	public:

		Editor(cage::Game& game);
		~Editor();

		void OnRevealed() override;
		void OnHidden() override;

		void ProcessEvents() override;
		void Update(float dt) override;
		void Draw() override;

	private:

		cage::ui::UIElement m_root;
		std::shared_ptr<cage::ui::LayoutGroup> m_tabBar, m_bottomPanel, m_rightPanel;
		std::shared_ptr<cage::ui::Button> m_play, m_pause, m_stop, m_hand, m_file, m_pencil;
		std::shared_ptr<TilePanel> m_tilePanel;
		std::shared_ptr<cage::ui::UIElement> m_tileHighlight, m_gameRoot;
		std::shared_ptr<cage::ui::DebugVisualizer> m_debugVisualizer;
		std::shared_ptr<cage::ui::CheckBox> m_gridToggle;

		cage::Font m_font;

		class Tool
		{
		protected:
			Editor* m_editor;
			World* m_world;
		public:
			Tool(Editor* editor, World* world) : m_editor(editor), m_world(world)
			{

			}

			virtual void Use(cage::Event&& e) = 0;
			bool inViewport(glm::ivec2 point)
			{
				return glm::all(lessThan(point, m_editor->m_gameViewport));
			}
		};

		class Pencil : public Tool
		{
			bool m_mouseDown;
			int m_radius;
			glm::ivec2 m_indices;

			void paint(int mouseX, int mouseY)
			{
				if (!inViewport({ mouseX, mouseY }))
					return;
				m_world->FillTileMouse(mouseX, mouseY, m_editor->m_tilePanel->GetSelection(), m_radius);

			}
		public:
			Pencil(Editor* editor, World* world) : Tool(editor, world), m_radius(0), m_mouseDown(false), m_indices(0)
			{

			}

			virtual void Use(cage::Event&& e) override
			{
				if (auto se = std::get_if<cage::ScrollEvent>(&e))
				{
					m_radius += se->amount;
					m_radius = std::max(m_radius, 0);
					//m_editor->m_tileHighlight->Resize(glm::vec2{ 2 * m_radius + 1, 2 * m_radius + 1 } *static_cast<float>(World::TILE_SIZE));

					m_editor->m_tileHighlight->Resize((float)World::TILE_SIZE * glm::vec2{ (float)2 * m_radius + 1 });
					m_editor->m_tileHighlight->SetTransform(m_world->GetViewMatrix() * glm::translate(glm::identity<glm::mat4>(), (float)World::TILE_SIZE * glm::vec3{ m_indices, 0 } + glm::vec3{ cage::ui::UIElement::GetMountOffset(cage::ui::MountPoint::TOP_LEFT, glm::vec2{World::TILE_SIZE}, { 1.f, 1.f }), 0.f }));

				}
				else if (auto ce = std::get_if<cage::MouseClickEvent>(&e))
				{
					m_mouseDown = !ce->wasRelease;
					paint(ce->x, ce->y);
				}
				else if (auto me = std::get_if<cage::MouseMotionEvent>(&e))
				{
					if (m_mouseDown)
						paint(me->x, me->y);

					m_editor->m_tileHighlight->SetVisible(inViewport({ me->x, me->y }));

					m_indices = m_world->ToTileIndices(me->x, me->y);
					m_editor->m_tileHighlight->Resize((float)World::TILE_SIZE * glm::vec2{ (float)2 * m_radius + 1 });
					m_editor->m_tileHighlight->SetTransform(m_world->GetViewMatrix() * glm::translate(glm::identity<glm::mat4>(), (float)World::TILE_SIZE * glm::vec3{ m_indices, 0 } + glm::vec3{ cage::ui::UIElement::GetMountOffset(cage::ui::MountPoint::TOP_LEFT, glm::vec2{World::TILE_SIZE}, { 1.f, 1.f }), 0.f }));
				}

				//->m_tileHighlight->SetScale({ m_world->GetZoom() });
			}
		};

		class Hand : public Tool
		{
			bool m_mouseDown;
		public:
			Hand(Editor* editor, World* world) : Tool(editor, world), m_mouseDown(false)
			{

			}

			virtual void Use(cage::Event&& e) override
			{
				if (auto ce = std::get_if<cage::MouseClickEvent>(&e))
				{
					if (inViewport({ ce->x, ce->y }))
						m_mouseDown = !ce->wasRelease;
					else if (ce->wasRelease)
						m_mouseDown = !ce->wasRelease;
				}
				else if (auto me = std::get_if<cage::MouseMotionEvent>(&e))
				{
					if (inViewport({me->x, me->y}) && m_mouseDown)
						m_world->Move(-glm::vec2{ me->dx, me->dy } / m_editor->m_world->GetZoom());
				}
				else if (auto se = std::get_if<cage::ScrollEvent>(&e))
				{
					m_world->Zoom(se->amount);
				}
			}
		};

		std::unique_ptr<World> m_world;
		std::unique_ptr<Tool> m_pencilTool, m_handTool;
		Tool* m_currentTool;

		glm::ivec2 m_gameViewport;

		bool m_playback;

	};
}