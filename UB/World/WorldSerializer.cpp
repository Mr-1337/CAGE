#include "WorldSerializer.hpp"

namespace ub
{
	WorldSerializer::WorldSerializer(World& world) : m_world(world)
	{
		
	}

	void WorldSerializer::LoadFromFile(const std::string& filename)
	{

	}

	void WorldSerializer::SaveToFile(const std::string& filename)
	{
		std::ofstream file("Assets/Worlds/" + filename);

		serializeTiles(file);
		serializeEntities(file);
		serializeStructures(file);
	}

	void WorldSerializer::serializeTiles(std::ofstream& file)
	{
		//file.write(reinterpret_cast<const char*>(&m_world.GetTileData()[0]), m_world.GetTileData().size());
	}

	void WorldSerializer::serializeEntities(std::ofstream& file)
	{
		// for each entity in the world
		// store the class type
		// serialize the entity (returns the binary blob)
		// dump the blob into the file as raw binary, prefixed with binary size in bytes

		file << std::endl << "Entities" << std::endl << "{" << std::endl;

		for (auto e : m_world.GetEntities())
		{
			file << '\t' << e->GetName() << std::endl;

		}

		file << "}";
	}

	void WorldSerializer::serializeStructures(std::ofstream& file)
	{

	}
}