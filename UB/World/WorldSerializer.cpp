#include "WorldSerializer.hpp"

namespace ub
{
	WorldSerializer::WorldSerializer(World& world) : m_world(world)
	{
		
	}

	void WorldSerializer::LoadFromFile(const std::string& filename)
	{
		std::ifstream file("Assets/Worlds/" + filename, std::ios::binary);

		deserializeTiles(file);
	}


	void WorldSerializer::SaveToFile(const std::string& filename)
	{
		std::ofstream file("Assets/Worlds/" + filename, std::ios::binary);

		serializeTiles(file);
		serializeEntities(file);
		serializeStructures(file);
	}

	void WorldSerializer::deserializeTiles(std::ifstream& file)
	{
		unsigned int w, h;
		file.read(reinterpret_cast<char*>(&w), sizeof(unsigned int));
		file.read(reinterpret_cast<char*>(&h), sizeof(unsigned int));

		m_world.Resize({ w, h });
		file.read(reinterpret_cast<char*>(&m_world.m_tilemap.m_tileData[0]), w * h * sizeof(Tilemap::Tile));
		m_world.m_tilemap.buildMesh();
	}

	void WorldSerializer::serializeTiles(std::ofstream& file)
	{
		auto [w, h] = m_world.m_tilemap.GetSize();

		file.write(reinterpret_cast<char*>(&w), sizeof(unsigned int));
		file.write(reinterpret_cast<char*>(&h), sizeof(unsigned int));
		file.write(reinterpret_cast<char*>(&m_world.m_tilemap.m_tileData[0]), m_world.m_tilemap.m_tileData.size() * sizeof(Tilemap::Tile));
	}

	void WorldSerializer::serializeEntities(std::ofstream& file)
	{
		// for each entity in the world
		// store the class type
		// serialize the entity (returns the binary blob)
		// dump the blob into the file as raw binary, prefixed with binary size in bytes
		for (auto e : m_world.GetEntities())
		{
			e->Serialize(file);
		}
	}

	void WorldSerializer::serializeStructures(std::ofstream& file)
	{

	}
}