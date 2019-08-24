#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "../Graphics/Vertex/Vertex.hpp"
#include "../Graphics/Models/Mesh.hpp"

namespace cage
{
	std::vector<Vertex3UVNormal> LoadObjVertices(const std::string& path);
	void LoadObjMeshes(const std::string& path, std::vector<Mesh<Vertex3UVNormal>>& meshesOut);
}