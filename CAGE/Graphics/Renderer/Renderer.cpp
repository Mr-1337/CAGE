#include "Renderer.hpp"
#include <Graphics/Vertex/Vertex.hpp>
#include <IO/VkMeshLoader.hpp>
#include <Graphics/UI/Button.hpp>
#include <SDL2/SDL_image.h>
#include <GLM/glm/gtx/string_cast.hpp>
#include <format>
#include <Graphics/Models/MeshFactory.hpp>

namespace cage
{
	namespace graphics
	{
		Renderer::Renderer(Device& device, std::pair<unsigned int, unsigned int> size, vk::SurfaceKHR surface) : 
			m_device(device), m_size(size), m_surface(surface), 
			m_uiVBO(false, device), m_minimized(false)
		{
			vk::Device dev = device;
			m_mouse = { 0.f, 0.f, 0.01f, 1.0f };
			m_Time = 0.0f;

			vk::Extent2D extent = {
				m_size.first,
				m_size.second
			};

			m_viewport = vk::Viewport(0.f, 0.f, extent.width, extent.height, 0.0f, 1.0f);
			m_renderScissor = vk::Rect2D({ 0, 0 }, extent);

			createSwapchain();
			createDepthBuffer();
			createRenderPass();
			createFramebuffers();
			createCommandBuffer();
			createSampler();
			buildPipeline();
			createUIResources();

			m_uboBuffer = new Buffer(device, sizeof(UBO), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
			std::vector<vk::DescriptorPoolSize> sizes = { vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1), vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1)};
			vk::DescriptorPoolCreateInfo dspInfo({}, 500, sizes);
			m_dsPool = dev.createDescriptorPool(dspInfo);

			std::vector<vk::DescriptorSetLayout> layouts = { m_uiDescriptorLayout };
			vk::DescriptorSetAllocateInfo dsAlloc(m_dsPool, layouts);
			auto dses = dev.allocateDescriptorSets(dsAlloc);
			m_uiDS = dses[0];

			VkTexture::s_Device = &device;

			vk::SemaphoreCreateInfo semInfo({});
			m_imageReady = dev.createSemaphore(semInfo);
			m_renderFinished = dev.createSemaphore(semInfo);
			vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);
			m_inFlight = dev.createFence(fenceInfo);
		}

		void Renderer::Init()
		{
			m_texture = std::make_shared<VkTexture>(IMG_Load("Assets/Textures/missing.png"));
			QueueTextureUpload(m_texture.get());
			m_texture->CreateDescriptorSet(m_dsPool, m_uiDescriptorLayout);

			StartOneTime();

			registerUIElement(m_root);

			EndOneTime();
		}

		void Renderer::renderScene(Scene& scene, int imageIndex)
		{
			/* 
			* sort DrawCmds by required state changes, minimizing expensive ones
			* sorting is done in this order
			* 1. Shader
			* 2. Material parameters
			* 3. Mesh
			* Switching shaders requires binding an entirely new graphics pipeline, as does switching rasterization settings, this will be fixed for given material categories (EG Diffuse, Phong, Glass)
			* Switching material parameters requires updating uniform values, and binding descriptors
			* Switching mesh requires binding vertex and index buffers
			*/
			auto drawCmds = scene.generateDraws();


			// Begin recording draw commands
			vk::CommandBufferBeginInfo recordInfo({}, nullptr);
			m_commandBuffer.begin(recordInfo);
			std::array<vk::ClearValue, 2> clearColors;
			auto bg = scene.m_bgColor;
			clearColors[0].setColor(vk::ClearColorValue().setFloat32({ bg.r, bg.g, bg.b, bg.a }));
			clearColors[1].setDepthStencil(vk::ClearDepthStencilValue({ 1.0f, 0 }));

			vk::RenderPassBeginInfo renderPassBeginInfo(m_renderPass, m_swapchainFramebuffers[imageIndex], m_renderScissor, clearColors);
			m_commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
			m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);
			m_commandBuffer.setViewport(0, m_viewport);
			m_commandBuffer.setScissor(0, m_renderScissor);
			//m_commandBuffer.pushConstants<glm::vec4>(m_pipelineLayout, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, m_mouse);

			// TODO: DrawCmd sorting, for now we will just assume every object goes into its own batch
			for (const auto& cmd : drawCmds)
			{


			}

			for (int cmdIndex = 0; cmdIndex < drawCmds.size(); cmdIndex++)
			{
				const auto cmd = drawCmds[cmdIndex];
				auto mesh = scene.m_meshes[cmd.MeshID].get();
				//auto material = scene.m_materials[cmd.MaterialID].get();
				m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, scene.m_descriptorManager.m_MeshDescriptors[cmdIndex], nullptr);
				m_commandBuffer.bindVertexBuffers(0, mesh->m_VertexBuffer.GetBuffer(), { 0 });
				m_commandBuffer.draw(mesh->m_VertexCount, 1, 0, 0);
			}
			renderUI();

			m_commandBuffer.endRenderPass();
			m_commandBuffer.end();

		}

		void Renderer::registerUIElement(Element e)
		{
			if (!e)
				return;
			if (e->m_VkTexture)
			{
				transitionImageLayout(e->m_VkTexture.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
				e->m_VkTexture->RecordTransfer(m_oneTimeBuffer);
				transitionImageLayout(e->m_VkTexture.get(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

				e->m_VkTexture->CreateDescriptorSet(m_dsPool, m_uiDescriptorLayout);
			}
			for (auto c : e->GetChildren())
				registerUIElement(c);
		}

		Renderer::~Renderer()
		{
			vk::Device dev = m_device;
			dev.waitIdle();
			for (auto u : m_uniformBuffers)
				delete u;
			for (auto& t : m_textures)
				t = nullptr;
			delete m_uboBuffer;
			
			destroySwapchain();
			dev.destroyRenderPass(m_renderPass);
			dev.destroySampler(m_textureSampler);
			dev.destroyDescriptorPool(m_dsPool);
			dev.destroyCommandPool(m_commandPool);
			dev.destroyDescriptorSetLayout(m_descriptorLayout);
			dev.destroyDescriptorSetLayout(m_uiDescriptorLayout);
			dev.destroyPipelineLayout(m_pipelineLayout);
			dev.destroyPipelineLayout(m_uiLayout);
			dev.destroyPipeline(m_uiPipeline);
			dev.destroyPipeline(m_graphicsPipeline);
			dev.destroyFence(m_inFlight);
			dev.destroySemaphore(m_imageReady);
			dev.destroySemaphore(m_renderFinished);
		}

		void Renderer::createSampler()
		{
			vk::Device dev = m_device;
			auto props = m_device.GetPhysical().getProperties();
			vk::SamplerCreateInfo info(vk::SamplerCreateFlags{ 0 }, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eNearest, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f, true, props.limits.maxSamplerAnisotropy / 2.0f, false, vk::CompareOp::eAlways, 0.0f, 0.0f);
			m_textureSampler = dev.createSampler(info);
		}

		void Renderer::createSwapchain()
		{
			vk::Device device = m_device;
			unsigned int familyIndex = m_device.m_familyIndex;

			vk::Extent2D extent = { 
				static_cast<unsigned int>(m_size.first), 
				static_cast<unsigned int>(m_size.second)
			};

			auto swapChainInfo = vk::SwapchainCreateInfoKHR({}, m_surface, 3, vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear, extent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, familyIndex, vk::SurfaceTransformFlagBitsKHR::eIdentity, vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eMailbox, true, nullptr);

			m_swapchain = device.createSwapchainKHR(swapChainInfo);

			std::vector<vk::Image> swapChainImages = device.getSwapchainImagesKHR(m_swapchain);

			for (int i = 0; i < swapChainImages.size(); i++)
			{
				vk::ComponentMapping componentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity);
				vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
				auto viewInfo = vk::ImageViewCreateInfo({}, swapChainImages[i], vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Srgb, componentMapping, subresource);
				m_swapchainViews.push_back(device.createImageView(viewInfo));
			}
		}

		void Renderer::createDepthBuffer()
		{
			vk::Device dev = m_device;
			// TODO: Make sure we check that we support the format we pick
			vk::Extent3D extent;
			extent.width = m_size.first;
			extent.height = m_size.second;
			extent.depth = 1;
			vk::Format depthFormat = vk::Format::eD24UnormS8Uint;
			vk::ImageCreateInfo info(vk::ImageCreateFlagBits{ 0 }, vk::ImageType::e2D, depthFormat, extent, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::SharingMode::eExclusive, nullptr, vk::ImageLayout::eUndefined);
			
			m_depthImage = dev.createImage(info);

			vk::MemoryRequirements memReq = dev.getImageMemoryRequirements(m_depthImage);
			vk::MemoryAllocateInfo allocInfo(memReq.size, m_device.FindMemoryType(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
			m_depthMemory = dev.allocateMemory(allocInfo);
			dev.bindImageMemory(m_depthImage, m_depthMemory, 0);

			vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
			vk::ComponentMapping swizzles;
			vk::ImageViewCreateInfo viewInfo(vk::ImageViewCreateFlags{ 0 }, m_depthImage, vk::ImageViewType::e2D, depthFormat, swizzles, subresource);
			m_depthView = dev.createImageView(viewInfo);

		}

		void Renderer::createFramebuffers()
		{
			vk::Device device = m_device;
			m_swapchainFramebuffers.resize(m_swapchainViews.size());

			for (int i = 0; i < m_swapchainFramebuffers.size(); i++)
			{
				std::vector<vk::ImageView> attachments = { m_swapchainViews[i], m_depthView };
				vk::FramebufferCreateInfo framebufferInfo({}, m_renderPass, attachments, m_size.first, m_size.second, 1);
				m_swapchainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
			}
		}

		void Renderer::createRenderPass()
		{
			vk::Device device = m_device;

			vk::AttachmentDescription colorAttachment({}, vk::Format::eB8G8R8A8Srgb, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
			vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eAttachmentOptimal);

			vk::Format depthFormat = vk::Format::eD24UnormS8Uint;

			vk::AttachmentDescription depthAttachment({}, depthFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
			vk::AttachmentReference depthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

			vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, nullptr, colorAttachmentRef, nullptr, &depthAttachmentRef, nullptr);

			vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests, {}, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
			std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
			vk::RenderPassCreateInfo renderPassCreateInfo({}, attachments, subpass, dependency);
			m_renderPass = device.createRenderPass(renderPassCreateInfo);
		}

		void Renderer::createCommandBuffer()
		{
			vk::Device device = m_device;
			vk::CommandPoolCreateInfo commandPoolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_device.m_familyIndex);

			m_commandPool = device.createCommandPool(commandPoolInfo);
			vk::CommandBufferAllocateInfo commandBufferInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, 1);
			m_commandBuffer = device.allocateCommandBuffers(commandBufferInfo)[0];
		}

		void Renderer::buildPipeline()
		{
			auto readFile = [](const std::string& filename)
			{
				std::ifstream file(filename, std::ios::ate | std::ios::binary);
				size_t fileSize = file.tellg();
				std::vector<unsigned int> buffer(fileSize / sizeof(unsigned int));
				file.seekg(0);
				file.read((char*)buffer.data(), fileSize);
				file.close();
				return buffer;
			};

			auto vertBin = readFile("Assets/Shaders/generic3D.vert.spv");
			auto fragBin = readFile("Assets/Shaders/generic3D.frag.spv");

			vk::Device device = m_device;
			PipelineBuilder builder(device, m_viewport, m_renderScissor);

			auto vs = std::make_shared<VkShader>(device, VkShader::ShaderType::VERTEX, vertBin);
			auto fs = std::make_shared<VkShader>(device, VkShader::ShaderType::FRAGMENT, fragBin);

			m_shaders = { vs, fs };

			builder.AddShader(*vs);
			builder.AddShader(*fs);
			builder.AddDepthTest();

			builder.AddRenderPass(m_renderPass);
			builder.SetVertexLayout(GetLayout<Vertex3UVNormal>(), sizeof(Vertex3UVNormal));
			//builder.SetTopology(vk::PrimitiveTopology::eTriangleStrip);

			vk::DescriptorSetLayoutBinding 
				ubo(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr), 
				sampler(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
			std::vector<vk::DescriptorSetLayoutBinding> layouts = { ubo, sampler };
			vk::DescriptorSetLayoutCreateInfo descriptor(vk::DescriptorSetLayoutCreateFlagBits{ 0 }, layouts);
			m_descriptorLayout = device.createDescriptorSetLayout(descriptor);
			builder.AddDescriptorLayout(m_descriptorLayout);

			m_graphicsPipeline = builder.Build();
			m_pipelineLayout = builder.m_pipelineLayout;
		}

		void Renderer::buildUIPipeline()
		{
			auto readFile = [](const std::string& filename)
			{
				std::ifstream file(filename, std::ios::ate | std::ios::binary);
				size_t fileSize = file.tellg();
				std::vector<unsigned int> buffer(fileSize / sizeof(unsigned int));
				file.seekg(0);
				file.read((char*)buffer.data(), fileSize);
				file.close();
				return buffer;
			};

			auto vertBin = readFile("Assets/Shaders/ui.vert.spv");
			auto fragBin = readFile("Assets/Shaders/ui.frag.spv");

			vk::Device device = m_device;
			PipelineBuilder builder(device, m_viewport, m_renderScissor);

			auto vs = std::make_shared<VkShader>(device, VkShader::ShaderType::VERTEX, vertBin);
			auto fs = std::make_shared<VkShader>(device, VkShader::ShaderType::FRAGMENT, fragBin);

			m_shaders = { vs, fs };

			builder.AddShader(*vs);
			builder.AddShader(*fs);
			//builder.AddDepthTest();

			builder.AddRenderPass(m_renderPass);
			builder.SetVertexLayout(GetLayout<Vertex2>(), sizeof(Vertex2));
			Uint32 offset = offsetof(UIPC, color);
			builder.AddPushConstantRange(vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(UIPC)));
			//builder.AddPushConstantRange(vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, 144));

			vk::DescriptorSetLayoutBinding sampler(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, &m_textureSampler);
			std::vector<vk::DescriptorSetLayoutBinding> layouts = { sampler };
			vk::DescriptorSetLayoutCreateInfo descriptor(vk::DescriptorSetLayoutCreateFlagBits{ 0 }, layouts);
			m_uiDescriptorLayout = device.createDescriptorSetLayout(descriptor);
			builder.AddDescriptorLayout(m_uiDescriptorLayout);

			m_uiPipeline = builder.Build();
			m_uiLayout = builder.m_pipelineLayout;
		}

		void Renderer::recordCommands(int imageIndex)
		{
			vk::CommandBufferBeginInfo recordInfo({}, nullptr);
			m_commandBuffer.begin(recordInfo);
			std::array<vk::ClearValue, 2> clearColors;
			clearColors[0].setColor(vk::ClearColorValue().setFloat32({ 1.0f, 0.0f, 0.0f, 1.0f }));
			clearColors[1].setDepthStencil(vk::ClearDepthStencilValue({ 1.0f, 0 }));

			vk::RenderPassBeginInfo renderPassBeginInfo(m_renderPass, m_swapchainFramebuffers[imageIndex], m_renderScissor, clearColors);
			m_commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
			m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);
			m_commandBuffer.setViewport(0, m_viewport);
			m_commandBuffer.setScissor(0, m_renderScissor);
			//m_commandBuffer.pushConstants<glm::vec4>(m_pipelineLayout, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, m_mouse);

			//m_commandBuffer.bindVertexBuffers(0, m_saul->m_VertexBuffer.GetBuffer(), { 0 });
			//m_commandBuffer.draw(m_saul->m_VertexCount, 1, 0, 0);
			renderUI();

			m_commandBuffer.endRenderPass();
			m_commandBuffer.end();
		}

		void Renderer::createUIResources()
		{
			std::vector<Vertex2> masterQuad;
			masterQuad.push_back(cage::Vertex2{ -0.5f, 0.5f });
			masterQuad.push_back(cage::Vertex2{ 0.5f, 0.5f });
			masterQuad.push_back(cage::Vertex2{ -0.5f, -0.5f });
			masterQuad.push_back(cage::Vertex2{ -0.5f, -0.5f });
			masterQuad.push_back(cage::Vertex2{ 0.5f, 0.5f });
			masterQuad.push_back(cage::Vertex2{ 0.5f, -0.5f });
			m_uiVBO.Fill(masterQuad.data(), sizeof(Vertex2) * masterQuad.size());

			buildUIPipeline();
		}

		void Renderer::destroySwapchain()
		{
			vk::Device dev = m_device;
			destroyDepthBuffer();
			for (auto& fb : m_swapchainFramebuffers)
			{
				dev.destroyFramebuffer(fb);
			}

			for (auto& view : m_swapchainViews)
			{
				dev.destroyImageView(view);
			}

			dev.destroySwapchainKHR(m_swapchain);
			m_swapchainViews.clear();
			m_swapchainFramebuffers.clear();
		}

		void Renderer::destroyDepthBuffer()
		{
			vk::Device dev = m_device;
			dev.destroyImage(m_depthImage);
			dev.destroyImageView(m_depthView);
			dev.freeMemory(m_depthMemory);
		}

		void Renderer::transitionImageLayout(VkTexture* texture, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
		{

			vk::PipelineStageFlagBits src, dst;

			vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			vk::ImageMemoryBarrier barrier(vk::AccessFlagBits{ 0 }, vk::AccessFlagBits{ 0 }, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, texture->m_image, subresource);

			if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
			{
				barrier.srcAccessMask = vk::AccessFlagBits{ 0 };
				barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
				src = vk::PipelineStageFlagBits::eTopOfPipe;
				dst = vk::PipelineStageFlagBits::eTransfer;
			}
			else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			{
				barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
				src = vk::PipelineStageFlagBits::eTransfer;
				dst = vk::PipelineStageFlagBits::eFragmentShader;
			}

			m_oneTimeBuffer.pipelineBarrier(src, dst, vk::DependencyFlags{ 0 }, nullptr, nullptr, barrier);
			
		}

		void Renderer::OnResize(std::pair<int, int> newSize)
		{
			io::Logger::Logf(io::Logger::Category::Graphics, io::Logger::Severity::Info, "Renderer resizing to {}x{}", newSize.first, newSize.second);
			m_size = newSize;
			if (m_minimized = m_size.first == 0 || m_size.second == 0)
			{
				return;
			}
			vk::Device dev = m_device;
			dev.waitIdle();


			vk::Extent2D extent = {
				static_cast<unsigned int>(m_size.first),
				static_cast<unsigned int>(m_size.second)
			};

			m_viewport = vk::Viewport(0.f, 0.f, extent.width, extent.height, 0.0f, 1.0f);
			m_renderScissor = vk::Rect2D({ 0, 0 }, extent);

			destroySwapchain();
			createSwapchain();
			createDepthBuffer();
			createFramebuffers();

		}

		void Renderer::Render(Scene& scene)
		{
			if (m_minimized)
				return;
			vk::Device dev = m_device;
			dev.waitForFences(m_inFlight, true, std::numeric_limits<uint64_t>::max());
			dev.resetFences(m_inFlight);

			while (!ui::UIElement::s_DirtyTexture.empty())
			{
				auto e = ui::UIElement::s_DirtyTexture.back();
				QueueTextureUpload(e->m_VkTexture.get());
				e->m_VkTexture->CreateDescriptorSet(m_dsPool, m_uiDescriptorLayout);
				//std::cout << "Dirty mode!\n";
				ui::UIElement::s_DirtyTexture.pop_back();
			}

			auto& texQueue = scene.m_transferQueue.Textures;
			if (!texQueue.empty())
			{
				StartOneTime();
				while (!texQueue.empty())
				{
					auto texture = texQueue.back();
					texQueue.pop_back();
					transitionImageLayout(texture, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
					texture->RecordTransfer(m_oneTimeBuffer);
					transitionImageLayout(texture, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
				}
				EndOneTime();
			}

			uint32_t imageIndex = dev.acquireNextImageKHR(m_swapchain, std::numeric_limits<uint64_t>::max(), m_imageReady, nullptr);
			m_mouse.z = m_Time;
			m_commandBuffer.reset({});
			//recordCommands(imageIndex);
			renderScene(scene, imageIndex);

			std::vector<vk::PipelineStageFlags> waitStages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
			vk::SubmitInfo submitInfo(m_imageReady, waitStages, m_commandBuffer, m_renderFinished);
			m_device.m_graphicsQueue.submit(submitInfo, m_inFlight);

			vk::PresentInfoKHR presentInfo(m_renderFinished, m_swapchain, imageIndex, nullptr);
			try
			{
				m_device.m_presentQueue.presentKHR(presentInfo);
			}
			catch (vk::OutOfDateKHRError e)
			{
				io::Logger::LogError(e.what(), io::Logger::Category::Graphics);
				m_minimized = true;
			}

		}

		void Renderer::renderUI()
		{
			m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_uiPipeline);
			m_commandBuffer.setViewport(0, m_viewport);
			m_commandBuffer.setScissor(0, m_renderScissor);

			m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_uiLayout, 0, m_texture->m_DescriptorSet, nullptr);

			m_commandBuffer.bindVertexBuffers(0, m_uiVBO.GetBuffer(), {0});
			m_uiPC.proj = glm::ortho(0.0f, (float)m_size.first, (float)m_size.second, 0.0f);

			if (m_root)
				renderUIElement(m_root);

		}

		void Renderer::renderUIElement(Element e)
		{
			m_uiPC.size = glm::vec4(e->GetSize(), 0.0, 1.0);
			m_uiPC.color = e->GetColor();
			m_uiPC.model = glm::identity<glm::mat4>();
			m_uiPC.model = e->GetTransform();
			m_uiPC.textured = { 0.0f, 0.0f, 0.0f, 0.0f };

			// bind texture
			if (e->m_VkTexture)
			{
				m_uiPC.textured = { 1.0f, 0.0f, 0.0f, 0.0f };
				vk::Device dev = m_device;
				m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_uiLayout, 0, e->m_VkTexture->m_DescriptorSet, nullptr);
			}

			m_commandBuffer.pushConstants<UIPC>(m_uiLayout, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, m_uiPC);
			m_commandBuffer.draw(6, 1, 0, 0);

			auto children = e->GetChildren();
			for (auto& c : children)
			{
				renderUIElement(c);
			}
		}

		void Renderer::StartOneTime()
		{
			vk::Device dev = m_device;
			vk::CommandBufferAllocateInfo info(m_commandPool, vk::CommandBufferLevel::ePrimary, 1);
			m_oneTimeBuffer = dev.allocateCommandBuffers(info)[0];
			vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr);
			m_oneTimeBuffer.begin(begin);
		}

		void Renderer::EndOneTime()
		{
			m_oneTimeBuffer.end();

			vk::SubmitInfo submit(nullptr, nullptr, m_oneTimeBuffer, nullptr);
			m_device.m_graphicsQueue.submit(submit, nullptr);
			m_device.m_graphicsQueue.waitIdle();
			m_device.operator vk::Device().freeCommandBuffers(m_commandPool, m_oneTimeBuffer);
		}

		void Renderer::QueueTextureUpload(VkTexture* texture)
		{
			//m_textureQueue.push_back(texture);
		}
	}
}