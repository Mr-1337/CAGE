#pragma once

#include <vector>
#include <GLM/glm/glm.hpp>
#include "../Renderer/DrawCmd.hpp"
#include "../Buffers/Buffer.hpp"
#include "../Textures/VkTexture.hpp"

namespace cage
{
	namespace graphics
	{
		// Responsible for managing allocation, lifetimes, and storing descriptors for scene objects. 
		class DescriptorManager final
		{
			Device& m_device;
			vk::DescriptorPool m_pool;
			vk::DescriptorSetLayout m_meshSetLayout;
			struct TransformUniform
			{
				glm::mat4 proj;
				glm::mat4 view;
				glm::mat4 model;
			};

			vk::Sampler m_textureSampler;

			std::vector<Buffer*> m_uniformBuffers;

		public:
			std::vector<vk::DescriptorSet> m_MeshDescriptors;
			Buffer m_UniformBuffer;
			DescriptorManager(Device& device);
			~DescriptorManager();
			void UpdateUniforms(const DrawCmd& drawObject, glm::mat4 viewProj);
			void UpdateObjectDescriptor(DrawCmd drawObject);
			void AllocObjectDescriptor(DrawCmd drawObject, VkTexture* texture);
		};
	}
}