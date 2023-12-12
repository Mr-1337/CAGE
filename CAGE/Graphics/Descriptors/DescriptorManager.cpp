#include "DescriptorManager.hpp"
#include <GLM/glm/gtc/matrix_transform.hpp>

namespace cage
{
	namespace graphics
	{
		DescriptorManager::DescriptorManager(Device& device) : m_device(device), m_UniformBuffer(device, sizeof(TransformUniform), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible)
		{
			// Shader resources are uber hardcoded here, in the future there should be a mechanism for dynamically specifying a shader's interface so these layouts may be generated automatically
			// Perhaps a future pipeline class that would contain all the required layout info?

			vk::Device vkDev = device;

			std::vector<vk::DescriptorPoolSize> sizes = { vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1), vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1) };
			vk::DescriptorPoolCreateInfo poolInfo({}, 16, sizes);
			m_pool = vkDev.createDescriptorPool(poolInfo);

			std::vector<vk::DescriptorSetLayoutBinding> bindings
			{ 
				{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex }, 
				{ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment } 
			};

			vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
			m_meshSetLayout = vkDev.createDescriptorSetLayout(layoutInfo);
			float maxAnisotropy = device.GetPhysical().getProperties().limits.maxSamplerAnisotropy;
			vk::SamplerCreateInfo samplerInfo(vk::SamplerCreateFlags{ 0 }, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eNearest, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f, true, maxAnisotropy / 2.0f, false, vk::CompareOp::eAlways, 0.0f, 0.0f);
			m_textureSampler = vkDev.createSampler(samplerInfo);

		}

		DescriptorManager::~DescriptorManager()
		{
			vk::Device vkDev = m_device;
			for (auto buffer : m_uniformBuffers)
			{
				delete buffer;
			}
			vkDev.destroySampler(m_textureSampler);
			vkDev.destroyDescriptorSetLayout(m_meshSetLayout);
			vkDev.destroyDescriptorPool(m_pool);
		}

		void DescriptorManager::UpdateUniforms(const DrawCmd& drawObject, glm::mat4 viewProj)
		{
			vk::Device vkDev = m_device;
			TransformUniform transform;
			transform.view = glm::identity<glm::mat4>();
			transform.proj = viewProj;
			transform.model = drawObject.SceneNode->m_GlobalTransform;

			void* mem = m_uniformBuffers[drawObject.ID]->Map();
			memcpy(mem, &transform, sizeof(TransformUniform));
			m_uniformBuffers[drawObject.ID]->Unmap();
		}

		void DescriptorManager::UpdateObjectDescriptor(DrawCmd drawObject)
		{

		}

		void DescriptorManager::AllocObjectDescriptor(DrawCmd drawObject, VkTexture* texture)
		{
			vk::Device vkDev = m_device;
			vk::DescriptorSetAllocateInfo dsAlloc(m_pool, m_meshSetLayout);
			auto objectDescriptor = vkDev.allocateDescriptorSets(dsAlloc)[0];
			m_MeshDescriptors.push_back(objectDescriptor);

			Buffer* uboBuffer = new Buffer(m_device, sizeof(TransformUniform), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
			m_uniformBuffers.push_back(uboBuffer);

			// write the descriptor sets
			vk::DescriptorBufferInfo dsBufferInfo(*m_uniformBuffers[drawObject.ID], 0, sizeof(TransformUniform));
			vk::DescriptorImageInfo imageInfo(m_textureSampler, texture->GetView(), vk::ImageLayout::eShaderReadOnlyOptimal);
			vk::WriteDescriptorSet dsWrite(objectDescriptor, 0, 0, vk::DescriptorType::eUniformBuffer, nullptr, dsBufferInfo, nullptr);
			vk::WriteDescriptorSet samplerDSWrite(objectDescriptor, 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo, nullptr, nullptr);
			std::vector<vk::WriteDescriptorSet> dsSet = { dsWrite, samplerDSWrite };
			vkDev.updateDescriptorSets(dsSet, nullptr);
		}
	}
}