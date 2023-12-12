#pragma once

#include <Graphics/Shaders/VkShader.hpp>
#include <Graphics/VertexLayouts/VertexAttribute.hpp>

namespace cage
{
	namespace graphics
	{
		// Responsible for the creation of the graphics pipeline. Simplifies the crazy amount of state management by dividing the pipeline creation into steps
		class PipelineBuilder
		{
			vk::Device& m_device;
			std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
			std::vector<vk::DescriptorSetLayout> m_dsLayouts;
			std::vector<vk::PushConstantRange> m_pcRanges;

			std::vector<vk::VertexInputAttributeDescription> m_vertexAttributes;
			std::vector<vk::VertexInputBindingDescription> m_vertexBindings;
			vk::PipelineVertexInputStateCreateInfo m_vertexInputState;
			vk::PipelineInputAssemblyStateCreateInfo m_iaState;
			vk::PipelineViewportStateCreateInfo m_viewportState;
			vk::PipelineRasterizationStateCreateInfo m_rasterState;
			vk::PipelineMultisampleStateCreateInfo m_msState;
			vk::PipelineDepthStencilStateCreateInfo m_depthStencilState;
			vk::PipelineColorBlendAttachmentState m_colorInfo;
			vk::PipelineColorBlendStateCreateInfo m_colorBlendState;

			void buildLayout();

		public:
			vk::PipelineLayout m_pipelineLayout;
			vk::RenderPass m_renderPass;

		public:
			PipelineBuilder(vk::Device& device, vk::Viewport viewport, vk::Rect2D scissor);
			~PipelineBuilder();

			void AddShader(const VkShader& shader);
			void AddRenderPass(vk::RenderPass rp);
			void AddDepthTest();
			void AddDescriptorLayout(vk::DescriptorSetLayout layout);
			void AddPushConstantRange(vk::PushConstantRange range);

			void SetVertexLayout(std::vector<VertexAttribute> layout, int vertSize);
			void SetTopology(vk::PrimitiveTopology topology);

			vk::Pipeline Build();
		};
	}
}