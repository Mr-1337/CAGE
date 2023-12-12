#pragma once
#include <vector>
#include <Graphics/Context/Device.hpp>
#include "Buffer.hpp"

namespace cage
{
	namespace graphics
	{
		class VkVertexBuffer
		{
		public:

			VkVertexBuffer(bool storeLocalCopy, Device& device) : m_device(device), m_local(storeLocalCopy), m_dev(device), m_buffer(nullptr)
			{

			}

			void Fill(void* data, size_t size)
			{
				m_buffer = new Buffer(m_device, size, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
				void* memMap = m_buffer->Map();
				memcpy(memMap, data, size);
				m_buffer->Unmap();
			}

			VkVertexBuffer(const VkVertexBuffer& other) = delete;

			inline vk::Buffer GetBuffer() { return *m_buffer; }

			~VkVertexBuffer()
			{
				delete m_buffer;
			}

			inline size_t GetSize() const { return m_size; }

		private:

			Device& m_device;
			vk::Device m_dev;
			
			Buffer* m_buffer;

			// Should we store the vertex data software side?
			bool m_local;

			size_t m_size;

		};
	}
}