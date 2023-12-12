#pragma once

#include <utility>
#include <fstream>
#include <GLM/glm/glm.hpp>
#include <Graphics/Context/Device.hpp>
#include <Graphics/Pipeline/PipelineBuilder.hpp>
#include <Graphics/Buffers/VkVertexBuffer.hpp>
#include <Graphics/Textures/VkTexture.hpp>
#include <Graphics/UI/UIElement.hpp>
#include <Graphics/Scene/Scene.hpp>
#include <IO/VkMeshLoader.hpp>
#include <IO/Logging/Logger.hpp>

namespace cage
{
	namespace graphics
	{
		class Renderer
		{
			
			using HTexture = std::shared_ptr<VkTexture>;
			int vertCount;

			Device& m_device;
			std::pair<unsigned int, unsigned int> m_size;
			vk::SurfaceKHR m_surface;
			VkVertexBuffer m_uiVBO;
			vk::Viewport m_viewport;
			vk::Rect2D m_renderScissor;
			std::vector<vk::ImageView> m_swapchainViews;
			std::vector<vk::Framebuffer> m_swapchainFramebuffers;
			std::vector<std::shared_ptr<VkShader>> m_shaders;

			std::vector<HTexture> m_textures;
			std::vector<Buffer*> m_uniformBuffers;

			vk::DescriptorSet m_uiDS;
			vk::DescriptorPool m_dsPool;
			Buffer *m_uboBuffer, *m_uboBuffer2;

			vk::Image m_depthImage;
			vk::DeviceMemory m_depthMemory;
			vk::ImageView m_depthView;

			vk::SwapchainKHR m_swapchain;
			vk::DescriptorSetLayout m_descriptorLayout, m_uiDescriptorLayout;
			vk::PipelineLayout m_pipelineLayout, m_uiLayout;
			vk::Pipeline m_graphicsPipeline;
			vk::Pipeline m_uiPipeline;
			vk::RenderPass m_renderPass;

			vk::CommandPool m_commandPool;
			vk::CommandBuffer m_commandBuffer, m_oneTimeBuffer;

			vk::Semaphore m_imageReady, m_renderFinished;
			vk::Fence m_inFlight;

			vk::Sampler m_textureSampler;

			HTexture m_texture;
			std::vector<HTexture> m_textureQueue;

			// Resource Creation
			void createSwapchain();
			void createDepthBuffer();
			void createRenderPass();
			void createFramebuffers();
			void createCommandBuffer();
			void createSampler();
			void buildPipeline();
			void buildUIPipeline();
			void createUIResources();

			void destroySwapchain();
			void destroyDepthBuffer();

			void recordCommands(int imageIndex);

			void transitionImageLayout(VkTexture* texture, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

			void renderScene(Scene& scene, int imageIndex);

			using Element = std::shared_ptr<ui::UIElement>;
			void registerUIElement(Element e);
			void renderUI();
			void renderUIElement(Element e);

			struct UBO
			{
				glm::mat4 proj;
				glm::mat4 view;
				glm::mat4 model;
			};

			std::vector<UBO> m_ubo;

			struct UIPC
			{
				glm::mat4 proj;
				glm::mat4 model;
				glm::vec4 size;
				glm::vec4 color;
				glm::vec4 textured;
			};

			UIPC m_uiPC;
			bool m_minimized;

			struct BatchData
			{

			};

		public:
			float m_Time;

			std::shared_ptr<ui::UIElement> m_root;
			glm::vec4 m_mouse;

			Renderer(Device& device, std::pair<unsigned int, unsigned int> size, vk::SurfaceKHR surface);
			~Renderer();

			void Init();

			void OnResize(std::pair<int, int> newSize);
			void Render(Scene& scene);

			void StartOneTime();
			void EndOneTime();

			void QueueTextureUpload(VkTexture* texture);
		};
	}
}