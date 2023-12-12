#include "VkMesh.hpp"
#include "../Context/Device.hpp"

namespace cage
{
	namespace graphics
	{
		class MeshFactory
		{
		public:
			// Builds a Cube of given sidelength
			static VkMesh* BuildCube(Device& dev, float size);
			// Builds a UV sphere of given radius and topology resolution
			static VkMesh* BuildSphere(Device& dev, float radius, unsigned int resolution = 10);
		};
	}
}