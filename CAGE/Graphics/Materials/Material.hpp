#include <GLM/glm/glm.hpp>
#include <string>
#include <vulkan/vulkan.hpp>

namespace cage
{
	namespace graphics
	{
		// A material in CAGE represents the specific configuration of state needed to render a mesh. This includes things like the shader to use, shader constants, textures that need to be bound, 
		class Material
		{
			std::string m_name;
			std::string m_shaderName;
			vk::Pipeline m_pipeline;

		};
	}
}