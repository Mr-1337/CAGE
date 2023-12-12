#pragma once

#include <GLM/glm/glm.hpp>
#include "../Scene/Node.hpp"

namespace cage
{
	namespace graphics
	{
		// Data required for CAGE to render a simple mesh. The IDs are used as indices into scene buffers to fetch required resources. The Node handle contains the transform info
		struct DrawCmd
		{
			unsigned int ID, MeshID, MaterialID;
			Node* SceneNode;
		};
	}
}