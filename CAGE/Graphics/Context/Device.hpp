#pragma once

// A graphics capable device ready to be used for rendering. This class facilitates interactions with the graphics driver and provides utilities for managing the available graphics devices.

namespace cage
{
	namespace graphics
	{
		class Device
		{
			vk::Device m_logicalDevice;
			vk::PhysicalDevice m_physicalDevice;
		public:
			vk::Queue m_graphicsQueue, m_presentQueue;
			unsigned int m_familyIndex;
			Device(vk::PhysicalDevice& physicalDevice);
			~Device();

			unsigned int FindMemoryType(unsigned int memoryTypeBits, vk::MemoryPropertyFlags properties);

			inline operator vk::Device()
			{
				return m_logicalDevice;
			}

			inline vk::PhysicalDevice GetPhysical()
			{
				return m_physicalDevice;
			}
		};

	}
}