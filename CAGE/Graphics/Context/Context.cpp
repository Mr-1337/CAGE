#include "Context.hpp"
#include "../../IO/Logging/Logger.hpp"

namespace cage
{
	namespace graphics
	{
		Context::Context(const std::string& appName, const Version_t appVersion, std::vector<const char*>& extensions)
		{
			using io::Logger;
			Logger::Log("Creating graphics context (Vulkan Instance)", Logger::Category::Graphics);
			auto instanceVersion = vk::enumerateInstanceVersion();
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Found Vulkan implementation version {}.{}.{}", VK_API_VERSION_MAJOR(instanceVersion), VK_API_VERSION_MINOR(instanceVersion), VK_API_VERSION_PATCH(instanceVersion));
			vk::ApplicationInfo appInfo(appName.c_str(), appVersion, "CAGE", EngineVersion, VK_API_VERSION_1_3);

			const std::vector <const char*> validationLayers =
			{
				"VK_LAYER_KHRONOS_validation"
			};

			vk::InstanceCreateInfo info({}, &appInfo, validationLayers, extensions);
			m_instance = vk::createInstance(info);
			findGPU();
		}

		void Context::findGPU()
		{
			vk::Device logicalDevice;
			std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
			for (auto& gpu : devices)
			{
				if (isSuitable(gpu))
				{
					m_device = std::make_unique<Device>(gpu);
					break;
				}
			}
		}

		bool Context::isSuitable(vk::PhysicalDevice& device)
		{
			using io::Logger;
			bool suitable = true;
			auto props = device.getProperties();
			auto features = device.getFeatures();


			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Device found: {}", props.deviceName.data());
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Max bound desciptor sets: {}", props.limits.maxBoundDescriptorSets);
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Max 2D image dimension: {}", props.limits.maxImageDimension2D);
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Max viewports: {}", props.limits.maxViewports);
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Device type: {}", vk::to_string(props.deviceType));
			if (props.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
			{
				Logger::LogWarn("Not discrete GPU, performance may be poor", Logger::Category::Graphics);
			}
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Supports multiviewport: {}", features.multiViewport);
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Supports geometry shader: {}", features.geometryShader);
			Logger::Logf(Logger::Category::Graphics, Logger::Severity::Info, "Supports widelines: {}", features.wideLines);

			return suitable;
		}

		Context::operator vk::Instance() const
		{
			return m_instance;
		}

		Context::~Context()
		{
			m_device = nullptr;
			m_instance.destroy();
		}
	}
}