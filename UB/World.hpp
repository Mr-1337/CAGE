#pragma once

#include "WorldShader.hpp"
#include "../CAGE/Graphics/Models/Mesh.hpp"


namespace ub
{
	class ScriptManager;

	class World
	{
	public:
		World(std::pair<int, int> winSize);
		~World();

		void Update(float dt);
		void Draw();
		void GenWorld();
		void HandleEvents(SDL_Event e);
		void MoveEnt(unsigned int id, glm::vec2 position);
		void SetWinSize(std::pair<int, int> winSize);
		void Resize(std::pair<int, int> worldSize);
		void Move(glm::vec2 position);
		void Zoom(int dy);
		
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

		const static int TILE_SIZE = 32;

		enum class Tile : unsigned char
		{
			INVALID = 0,
			GRASS,
			SAND,
			DIRT,
			WATER,
			LIGHT_GRASS,
			LAVA,
			ROCK_DARK,
			ROCK_LIGHT,
			PURPLE,
			TILE_COUNT = 48
		};

		void PaintTileMouse(int mouseX, int mouseY, Tile type);
		void FillTileMouse(int mouseX, int mouseY, Tile type, int radius);
		void PaintTileRaw(int tileX, int tileY, Tile type);

		glm::ivec2 ToTileIndices(int x, int y);

	private:
		glm::mat4 calcViewMatrix();

		std::shared_ptr<ub::WorldShader> m_worldShader;
		cage::Mesh<cage::Vertex2UV> m_worldMesh, m_playerMesh, m_uberMesh, m_gridMesh;

		glm::mat4 m_projection, m_view;
		glm::vec2 m_cameraPos, m_position, m_pos2, m_velocity;
		float m_zoom; 
		bool m_drawGrid, m_dirty;

		ScriptManager* m_scriptManager;

		std::vector<Tile> m_tileData;
		std::vector<cage::Vertex2UV> m_hackBuffer;

		std::pair<int, int> m_winSize, m_worldSize;

	};

}