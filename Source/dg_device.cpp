#include "dg_logger.hpp"
#include "dg_device.hpp"

// std
#include <stdexcept>
#include <set>

namespace dg
{

	Device::Device(vk::Instance& instance, const std::vector<const char*>& extensions)
		: m_instance(instance), m_extensions(extensions) {}

	Device::~Device()
	{
		// device.destroy(nullptr);
	}
	
	void Device::pickPhysicalDevice()
	{

		std::vector<vk::PhysicalDevice> availableDevices = m_instance.enumeratePhysicalDevices();

		for (const auto& device : availableDevices)
		{
			if (areExtensionsSupportedBy(device))
			{
				physical = device;
				Logger::msgLn("Physical device was correctly choosen");
				return;
			}
		}

		throw std::runtime_error("No devices are suitable");
	}
	
	void Device::createLogicalDevice()
	{
		uint32_t graphicsQueueFamilyIndex = findQueueFamilyIndex(vk::QueueFlagBits::eGraphics);

		float queuePriority = 1.0f;
		vk::DeviceQueueCreateInfo queueCreateInfo(
				vk::DeviceQueueCreateFlags(), graphicsQueueFamilyIndex, 1, &queuePriority
				);

		vk::PhysicalDeviceFeatures deviceFeatures;

		vk::DeviceCreateInfo deviceInfo(
				vk::DeviceCreateFlags(),
				1, &queueCreateInfo,
				0, nullptr, // enabledLayers count & names, deprecated and ignored (see doc)
				m_extensions.size(), m_extensions.data(),
				&deviceFeatures
				);

		device = physical.createDevice(deviceInfo);
		Logger::msgLn("Logical device created");
	}

	bool Device::areExtensionsSupportedBy(const vk::PhysicalDevice& physicalDevice)
	{
			std::vector<vk::ExtensionProperties> availableExtensions =
				physicalDevice.enumerateDeviceExtensionProperties();

			std::cout << m_extensions[0] << std::endl;
			std::set<std::string> requiredExtensions(m_extensions.begin(), m_extensions.end());
			std::cout << requiredExtensions.size() << std::endl;

			for (const auto &extension : availableExtensions) {
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
	}

	uint32_t Device::findQueueFamilyIndex(vk::QueueFlags queueType)
	{
		std::vector<vk::QueueFamilyProperties> queueFamilies = physical.getQueueFamilyProperties();

		for (uint32_t i = 0 ; i < queueFamilies.size() ; i++)
		{
			if ((queueFamilies[i].queueCount > 0) && (queueFamilies[i].queueFlags & queueType))
				return i;
		}

		throw std::runtime_error("Could't find a suitable queue family");
	}
	
		
} /* dg */ 
