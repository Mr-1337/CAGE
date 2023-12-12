#include "VkTexture.hpp"
#include <iostream>

namespace cage
{
	namespace graphics
	{

		VkTexture* VkTexture::s_MissingTexture;
		Device* VkTexture::s_Device;

		VkTexture::VkTexture(SDL_Surface* textureData, bool keepLocalCopy) :
			m_size(textureData->w, textureData->h),
			m_surface(nullptr)
		{
			auto format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
			m_surface = SDL_ConvertSurface(textureData, format, 0);
			SDL_FreeFormat(format);
			SDL_FreeSurface(textureData);
			unsigned int dataSize = m_size.first * m_size.second * m_surface->format->BytesPerPixel;
			vk::Device dev = *s_Device;
			m_stagingBuffer = new Buffer(*s_Device, dataSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			void* memMap = m_stagingBuffer->Map();
			memcpy(memMap, m_surface->pixels, dataSize);
			m_stagingBuffer->Unmap();

			vk::Extent3D extent(m_size.first, m_size.second, 1);
			vk::ImageCreateInfo imageInfo(vk::ImageCreateFlags{ 0 }, vk::ImageType::e2D, 
				vk::Format::eR8G8B8A8Srgb, extent, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, 
				vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::SharingMode::eExclusive, nullptr, vk::ImageLayout::eUndefined);

			m_image = dev.createImage(imageInfo);

			vk::MemoryRequirements memReq = dev.getImageMemoryRequirements(m_image);
			vk::MemoryAllocateInfo allocInfo(memReq.size, s_Device->FindMemoryType(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
			m_texMemory = dev.allocateMemory(allocInfo);
			dev.bindImageMemory(m_image, m_texMemory, 0);

			vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			vk::ComponentMapping swizzles;
			vk::ImageViewCreateInfo viewInfo(vk::ImageViewCreateFlags{ 0 }, m_image, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb, swizzles, subresource);
			m_view = dev.createImageView(viewInfo);
		}

		VkTexture::~VkTexture()
		{
			vk::Device dev = *s_Device;
			dev.freeMemory(m_texMemory);
			dev.destroyImageView(m_view);
			dev.destroyImage(m_image);
			SDL_FreeSurface(m_surface);
			delete m_stagingBuffer;
		}

		void VkTexture::CreateDescriptorSet(vk::DescriptorPool pool, vk::DescriptorSetLayout layout)
		{
			vk::Device dev = *s_Device;

			vk::DescriptorSetAllocateInfo alloc(pool, layout);
			m_DescriptorSet = dev.allocateDescriptorSets(alloc)[0];
			vk::DescriptorImageInfo imageInfo(nullptr, m_view, vk::ImageLayout::eShaderReadOnlyOptimal);
			vk::WriteDescriptorSet samplerDSWrite(m_DescriptorSet, 0, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo, nullptr, nullptr);
			std::vector<vk::WriteDescriptorSet> dsSet = { samplerDSWrite };
			dev.updateDescriptorSets(dsSet, nullptr);
		}

		void VkTexture::RecordTransfer(vk::CommandBuffer& cmd)
		{
			vk::Device dev = *s_Device;
			vk::DeviceSize size = 0;
			vk::ImageSubresourceLayers subresource(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
			vk::Extent3D extent{ m_size.first, m_size.second, 1 };
			vk::BufferImageCopy region(0, 0, 0, subresource, vk::Offset3D{0, 0, 0}, extent);
			cmd.copyBufferToImage(*m_stagingBuffer, m_image, vk::ImageLayout::eTransferDstOptimal, region);
		}

	}
}