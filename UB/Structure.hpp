#pragma once

#include <memory>
#include "../CAGE/Graphics/Models/Mesh.hpp"

namespace ub
{
	struct Structure
	{
		Structure();
		std::shared_ptr<cage::Texture> texture;
		std::shared_ptr<cage::Mesh<cage::Vertex3UV>> mesh;
		int x, y, w, h;
	};
}