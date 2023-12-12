#include "Buffer.hpp"

namespace cage
{
	namespace graphics
	{
		Buffer::Buffer(Device& device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memFlags) : m_device(device), m_size(size), m_dev(device)
		{
			m_size = size;
			vk::BufferCreateInfo info(vk::BufferCreateFlagBits{ 0 }, m_size, usage, vk::SharingMode::eExclusive, {});
			vk::Device dev = m_device;
			m_buffer = dev.createBuffer(info);

			vk::MemoryRequirements memReq = dev.getBufferMemoryRequirements(m_buffer);
			unsigned int memType = m_device.FindMemoryType(memReq.memoryTypeBits, memFlags);

			vk::MemoryAllocateInfo alloc(memReq.size, memType);
			m_memory = dev.allocateMemory(alloc);
			dev.bindBufferMemory(m_buffer, m_memory, 0);
		}

		Buffer::~Buffer()
		{
			m_dev.freeMemory(m_memory);
			m_dev.destroyBuffer(m_buffer);
		}

		void* Buffer::Map()
		{
			return m_dev.mapMemory(m_memory, 0, m_size, vk::MemoryMapFlags{ 0 });
		}

		void Buffer::Unmap()
		{
			m_dev.unmapMemory(m_memory);
		}
	}
}