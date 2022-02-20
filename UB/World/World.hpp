#pragma once

#include <utility>
#include "WorldShader.hpp"
#include "Tilemap.hpp"
#include "Structure.hpp"
#include "../CAGE/Graphics/Models/Mesh.hpp"
#include "../Character.hpp"
#include "../Player.hpp"
#include "../Battle.hpp"

namespace ub
{
	class ScriptManager;

	class World
	{
		friend class WorldSerializer;

	public:
		World(std::pair<int, int> winSize);
		~World();

		void Update(float dt);
		void Draw();
		void GenWorld();
		void HandleEvents(cage::Event&& e);

		std::vector<Entity*> GetEntities();

		auto GetDialogueBox() { return m_dialogue; }

		inline Player* GetPlayer() { return m_player; }
		inline ScriptManager* GetScriptManager() { return m_scriptManager; }

		void MoveEnt(unsigned int id, glm::vec2 position);

		void SetWinSize(std::pair<int, int> winSize);
		inline void SetControl(bool control) { m_control = control; }

		void Resize(std::pair<int, int> worldSize);
		void Move(glm::vec2 position);
		void Zoom(int dy);

		void StartBattle(Enemy* enemy);
		inline bool InBattle() { return m_battle != nullptr; }

		inline std::pair<unsigned int, unsigned int> GetSize() { return m_tilemap.GetSize(); }
		
		inline glm::vec2 GetCameraPos() 
		{ 
			glm::vec2 camOffset = { m_winSize.first / 2, m_winSize.second / 2 };
			camOffset /= m_zoom;
			return m_cameraPos + camOffset; 
		}
		inline glm::mat4 GetViewMatrix() { return m_view; }
		inline float GetZoom() { return m_zoom; }
		inline void SetGrid(bool grid) { m_drawGrid = grid; }

		void Reload();

		void PaintTileMouse(int mouseX, int mouseY, Tilemap::Tile type);
		void FillTileMouse(int mouseX, int mouseY, Tilemap::Tile type, int radius);
		void PaintTileRaw(int tileX, int tileY, Tilemap::Tile type);

		glm::ivec2 ToTileIndices(int x, int y);

	private:
		glm::mat4 calcViewMatrix();

		std::shared_ptr<ub::WorldShader> m_worldShader;

		cage::ui::UIElement m_root;
		std::shared_ptr<DialogueBox> m_dialogue;

		Tilemap m_tilemap;
		cage::Mesh<cage::Vertex3UV> m_gridMesh;

		glm::mat4 m_projection, m_view;
		glm::vec2 m_cameraPos, m_pos2, m_velocity;
		float m_zoom; 
		bool m_drawGrid, m_dirty, m_control;

		ScriptManager* m_scriptManager;
		std::unique_ptr<Battle> m_battle;

		Structure house;

		Player* m_player;
		Character* m_dev;
		Enemy* m_enemy;

		std::vector<Structure> m_structureData;
		std::vector<Entity*> m_entities;
		std::vector<cage::Vertex3UV> m_hackBuffer;

		std::pair<int, int> m_winSize;

	};

}