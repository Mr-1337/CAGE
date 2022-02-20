#pragma once

#include <Graphics/Models/Mesh.hpp>

namespace ub
{
	// Class containing the tile data and mesh and provides facilities for sampling a tile
	class Tilemap
	{
		friend class WorldSerializer;
	public:

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

		void Draw();

		Tilemap(unsigned int width, unsigned int height);
		~Tilemap();

		void UpdateMesh(unsigned int x, unsigned int y);
		void RegenMesh();

		Tilemap& operator= (const Tilemap& rhs);
		Tile& operator() (unsigned int x, unsigned int y);
		Tile& operator() (glm::vec2 position);

		inline std::pair<unsigned int, unsigned int> GetSize() const { return { m_width, m_height }; }

	private:

		unsigned int m_width, m_height;

		void buildMesh();
		void makeTile(std::vector<cage::Vertex3UV>& geometry, int x, int y, unsigned char tileType);

		std::vector<Tile> m_tileData;
		cage::Mesh<cage::Vertex3UV> m_mesh;
	};
}