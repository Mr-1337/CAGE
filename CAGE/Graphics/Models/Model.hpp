#pragma once

#include <vector>

#include "Mesh.hpp"
#include "../../IO/MeshLoader.hpp"

namespace cage
{
	class Model
	{
	protected:
		std::vector<Mesh<Vertex3UVNormal>> m_meshes;
	public:

		Model() = default;
		~Model() = default;

		void LoadModel(const std::string& path)
		{
			m_meshes.reserve(50);
			LoadObjMeshes(path, m_meshes);
		}

		virtual void Draw()
		{
			for (auto& m : m_meshes)
			{
				m.Draw();
			}
		}
	};
}