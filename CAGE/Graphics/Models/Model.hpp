#pragma once

#include <vector>

#include "Mesh.hpp"
#include "../../IO/MeshLoader.hpp"

namespace cage
{
	class Model
	{
		std::vector<Mesh<Vertex3UVNormal>> m_meshes;
	public:

		Model() = default;
		~Model() = default;

		void LoadModel(const std::string& path)
		{
			m_meshes.reserve(10);
			LoadObjMeshes(path, m_meshes);
		}

		void Draw()
		{
			for (auto& m : m_meshes)
			{
				m.Draw();
			}
		}
	};
}