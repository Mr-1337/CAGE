#include "PipelineBuilder.hpp"

namespace cage
{
	namespace graphics
	{
		PipelineBuilder::PipelineBuilder(vk::Device& device, vk::Viewport viewport, vk::Rect2D scissor) : m_device(device)
		{
			m_vertexInputState = vk::PipelineVertexInputStateCreateInfo({}, nullptr, nullptr);
			m_iaState = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, false);

			m_viewportState = vk::PipelineViewportStateCreateInfo({}, viewport, scissor);
			m_rasterState = vk::PipelineRasterizationStateCreateInfo({}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, false, 0.0f, 0.0f, 0.0f, 1.0f);
			m_msState = vk::PipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false);
			m_colorInfo = vk::PipelineColorBlendAttachmentState (true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
			m_colorBlendState = vk::PipelineColorBlendStateCreateInfo({}, false, vk::LogicOp::eCopy, m_colorInfo, { 0.0f, 0.0f, 0.0f, 0.0f });
			m_depthStencilState = vk::PipelineDepthStencilStateCreateInfo({}, false, false, vk::CompareOp::eLess, false, false, {}, {}, 0.0f, 1.0f);


		}

		PipelineBuilder::~PipelineBuilder()
		{

		}

		void PipelineBuilder::AddShader(const VkShader& shader)
		{
			vk::PipelineShaderStageCreateInfo shaderStage((vk::PipelineShaderStageCreateFlagBits)0, (vk::ShaderStageFlagBits)shader.k_Type, shader.GetModule(), "main");
			m_shaderStages.push_back(shaderStage);
		}

		void PipelineBuilder::AddRenderPass(vk::RenderPass rp)
		{
			m_renderPass = rp;
		}

		void PipelineBuilder::AddDepthTest()
		{
			m_depthStencilState = vk::PipelineDepthStencilStateCreateInfo({}, true, true, vk::CompareOp::eLess, false, false, {}, {}, 0.0f, 1.0f);
		}

		void PipelineBuilder::SetTopology(vk::PrimitiveTopology topology)
		{
			m_iaState.topology = topology;
		}

		void PipelineBuilder::AddDescriptorLayout(vk::DescriptorSetLayout layout)
		{
			m_dsLayouts.push_back(layout);
		}

		void PipelineBuilder::AddPushConstantRange(vk::PushConstantRange range)
		{
			m_pcRanges.push_back(range);
		}

		void PipelineBuilder::SetVertexLayout(std::vector<VertexAttribute> layout, int vertSize)
		{
			m_vertexAttributes.clear();
			int location = 0;
			for (const auto& attribute : layout)
			{
				m_vertexAttributes.push_back(vk::VertexInputAttributeDescription(location++, 0, attribute.Format, attribute.offset));
			}
			m_vertexBindings = { vk::VertexInputBindingDescription(0, vertSize, vk::VertexInputRate::eVertex) };
			m_vertexInputState = vk::PipelineVertexInputStateCreateInfo({}, m_vertexBindings, m_vertexAttributes);
		}

		void PipelineBuilder::buildLayout()
		{
			vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, m_dsLayouts, m_pcRanges);
			m_pipelineLayout = m_device.createPipelineLayout(pipelineLayoutInfo);
		}

		vk::Pipeline PipelineBuilder::Build()
		{
			buildLayout();

			std::vector d{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };
			auto dynamicState = vk::PipelineDynamicStateCreateInfo((vk::PipelineDynamicStateCreateFlags)0, d);
			vk::GraphicsPipelineCreateInfo pipelineInfo({}, m_shaderStages, &m_vertexInputState, &m_iaState, nullptr, &m_viewportState, &m_rasterState, &m_msState, &m_depthStencilState, &m_colorBlendState, &dynamicState, m_pipelineLayout, m_renderPass, 0, nullptr, -1);
			vk::Pipeline pipeline = m_device.createGraphicsPipeline(nullptr, pipelineInfo);
			return pipeline;
		}
	}
}