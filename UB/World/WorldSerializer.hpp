#pragma once

#include <fstream>
#include "World.hpp"

namespace ub
{
	class WorldSerializer
	{
	public:
		WorldSerializer(World& world);

		void LoadFromFile(const std::string& filename);
		void SaveToFile(const std::string& filename);

	private:
		World& m_world;

		void deserializeTiles(std::ifstream& file);
		void deserializeEntities(std::ifstream& file);
		void deserializeStructures(std::ifstream& file);

		void serializeTiles(std::ofstream& file);
		void serializeEntities(std::ofstream& file);
		void serializeStructures(std::ofstream& file);

	};
}