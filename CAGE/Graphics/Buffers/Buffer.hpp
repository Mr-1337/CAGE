#pragma once

#include <Graphics/Context/Device.hpp>

namespace cage
{
	namespace graphics
	{
		class Buffer
		{
			Device& m_device;
			size_t m_size;
			vk::Buffer m_buffer;
			vk::DeviceMemory m_memory;
			vk::Device m_dev;
		public:
			Buffer(Device& device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memFlags);
			~Buffer();

			void* Map();
			void Unmap();

			inline operator vk::Buffer() { return m_buffer; }
		private:

		};
	}
}