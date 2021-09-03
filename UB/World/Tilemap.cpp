#include "Tilemap.hpp"
#include <algorithm>

namespace ub
{
	Tilemap::Tilemap(unsigned int width, unsigned int height) : m_mesh("World"), m_width(width), m_height(height)
	{
		auto surface = IMG_Load("Assets/Textures/UBatlas.png");
		m_mesh.LoadTexture(surface);
		SDL_FreeSurface(surface);

		m_tileData.resize(m_width * m_height);
		std::for_each(std::begin(m_tileData), std::end(m_tileData), [](auto& t) { t = Tile::LAVA; });

		buildMesh();
	}

	Tilemap::~Tilemap()
	{

	}

	Tilemap& Tilemap::operator=(const Tilemap& rhs)
	{
		m_tileData = rhs.m_tileData;
		m_width = rhs.m_width;
		m_height = rhs.m_height;

		buildMesh();
		return *this;
	}

	void Tilemap::makeTile(std::vector<cage::Vertex3UV>& geometry, int x, int y, unsigned char tileType)
	{
		float size = 1.0f / (16);
		float uvx = (float)(tileType % 16) * size;
		float uvy = (float)(tileType / 16) * size;

		geometry.emplace_back(x * TILE_SIZE, y * TILE_SIZE, 0, uvx, uvy);
		geometry.emplace_back((x + 1) * TILE_SIZE, y * TILE_SIZE, 0, uvx + size, uvy);
		geometry.emplace_back(x * TILE_SIZE, (y + 1) * TILE_SIZE, 0, uvx, uvy + size);

		geometry.emplace_back((x + 1) * TILE_SIZE, y * TILE_SIZE, 0, uvx + size, uvy);
		geometry.emplace_back(x * TILE_SIZE, (y + 1) * TILE_SIZE, 0, uvx, uvy + size);
		geometry.emplace_back((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE, 0, uvx + size, uvy + size);
	}

	void Tilemap::buildMesh()
	{
		std::vector<cage::Vertex3UV> geometry;
		geometry.reserve(m_width * m_height * 6);

		for (int y = 0; y < m_height; y++)
			for (int x = 0; x < m_width; x++)
				makeTile(geometry, x, y, static_cast<unsigned char>(m_tileData[y * m_width + x]));

		m_mesh.SetGeometry(geometry);
	}

	void Tilemap::UpdateMesh(unsigned int x, unsigned int y)
	{
		int index = (y * m_width + x) * 6;
		std::vector<cage::Vertex3UV> geometry;
		geometry.reserve(30);
		makeTile(geometry, x, y, static_cast<unsigned char>(operator()(x, y)));
		m_mesh.GetBuffer().UpdateRange(index, geometry);
	}

	void Tilemap::RegenMesh()
	{
		buildMesh();
	}

	Tilemap::Tile& Tilemap::operator() (unsigned int x, unsigned int y)
	{
		return m_tileData[y * m_width + x];
	}

	Tilemap::Tile& Tilemap::operator() (glm::vec2 position)
	{
		position = glm::clamp(position, { 0.0f, 0.0f }, { static_cast<float>(m_width), static_cast<float>(m_height) });
		return operator()(static_cast<unsigned int>(position.x), static_cast<unsigned int>(position.y));
	}

	void Tilemap::Draw()
	{
		m_mesh.Draw();
	}
}