#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "../Graphics/Vertex/Vertex.hpp"

namespace cage
{
	std::vector<Vertex3UVNormal> loadObjVertices(const std::string& path);
}