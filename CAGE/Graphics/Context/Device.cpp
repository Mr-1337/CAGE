#include "Device.hpp"

namespace cage
{
	namespace graphics
	{
		Device::Device(vk::PhysicalDevice& physicalDevice): m_familyIndex(0), m_physicalDevice(physicalDevice)
		{
			auto queueFamilies = physicalDevice.getQueueFamilyProperties();

			unsigned int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
				{
					m_familyIndex = i;
				}
				i++;
			}

			std::vector<float> priorities = { 1.0f };
			vk::DeviceQueueCreateInfo queueInfo({}, m_familyIndex, priorities);
			std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			vk::PhysicalDeviceFeatures requested;
			requested.samplerAnisotropy = true;
			requested.multiViewport = true;
			vk::DeviceCreateInfo createInfo({}, queueInfo, nullptr, deviceExtensions, &requested);
			m_logicalDevice = physicalDevice.createDevice(createInfo);

			m_graphicsQueue = m_logicalDevice.getQueue(m_familyIndex, 0);
			m_presentQueue = m_logicalDevice.getQueue(m_familyIndex, 0);
		}

		Device::~Device()
		{
			m_logicalDevice.destroy();
		}

		unsigned int Device::FindMemoryType(unsigned int memoryTypeBits, vk::MemoryPropertyFlags properties)
		{
			auto memProps = m_physicalDevice.getMemoryProperties();

			for (unsigned int i = 0; i < memProps.memoryTypeCount; i++)
			{
				const bool isType = memoryTypeBits & (1 << i);
				const bool hasProps = (memProps.memoryTypes[i].propertyFlags & properties) == properties;
				if (isType && hasProps)
				{
					return i;
				}
			}
			throw std::runtime_error("Couldn't find suitable memory type!");
		}
	}
}