#pragma once

#include "../Graphics/Models/Mesh.hpp"

namespace cage
{
	
	class Model;

	std::vector<Vertex3UVNormal> LoadObjVertices(const std::string& path);
	void LoadObjMeshes(const std::string& path, std::vector<Mesh<Vertex3UVNormal>*>& meshesOut);
	Model* LoadModelFromFile(const std::string& path);
}