#pragma once

#include <Version.hpp>
#include "Device.hpp"

// Represents the graphics context. Wraps the VkInstance struct

namespace cage
{
	namespace graphics
	{
		class Context
		{
			vk::Instance m_instance;

			// Queries system for all connected devices and selects the most appropriate
			void findGPU();
			bool isSuitable(vk::PhysicalDevice& device);

		public:
			std::unique_ptr<Device> m_device;
			Context(const std::string& appName, const Version_t appVersion, std::vector<const char*>& extensions);
			~Context();

			operator vk::Instance() const;
		};
	}
}