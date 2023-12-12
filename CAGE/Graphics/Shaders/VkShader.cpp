#include "VkShader.hpp"

namespace cage
{
	namespace graphics
	{
		VkShader::VkShader(vk::Device device, ShaderType type, const std::vector<unsigned int>& bytecode) : k_Type(type), m_device(device)
		{
			vk::ShaderModuleCreateInfo info(vk::ShaderModuleCreateFlagBits{ 0 }, bytecode);
			m_module = m_device.createShaderModule(info);
		}

		VkShader::~VkShader()
		{
			m_device.destroyShaderModule(m_module);
		}
	}
}