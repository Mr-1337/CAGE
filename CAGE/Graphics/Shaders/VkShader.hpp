#pragma once

#include <vector>

namespace cage
{
	namespace graphics
	{
		class VkShader
		{
		public:

			enum class ShaderType
			{
				VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
				FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
				COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT
			};

			VkShader(vk::Device device, ShaderType type, const std::vector<unsigned int>& bytecode);
			~VkShader();

			inline vk::ShaderModule GetModule() const { return m_module; }

			const ShaderType k_Type;

		private:

			vk::Device m_device;
			vk::ShaderModule m_module;

		};
	}
}