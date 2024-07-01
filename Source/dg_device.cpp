#include "dg_logger.hpp"

// std
#include <stdexcept>

namespace dg
{

	namespace device
	{

		bool isSupporting(
				const vk::PhysicalDevice& physicalDevice,
				const std::span<const char *>& requestedExtensions
				)
		{

			std::vector<vk::ExtensionProperties> availableExtensions =
				physicalDevice.enumerateDeviceExtensionProperties();

			for (const char* requestedExtension : requestedExtensions)
			{
				bool isSupported = false;

				for (const auto& extension : availableExtensions)
				{
					std::string name = static_cast<std::string>(extension.extensionName);
					if (!name.compare(requestedExtension))
					{
						isSupported = true;
						break;
					}
				}

				if (!isSupported) return false;

			}

			return true;
		}

		bool isSuitable(const vk::PhysicalDevice physicalDevice)
		{

			const char* requestedExtensions[] = {vk::KHRSwapchainExtensionName};

			if (isSupporting(physicalDevice, requestedExtensions))
				return true;

			Logger::msgLn("Device do not support requested extensions");
			return false;

		}

		vk::PhysicalDevice choosePhysicalDevice(const vk::Instance instance)
		{

			std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

			for (const auto& device : availableDevices)
			{
				if (isSuitable(device)) return device;
			}

			throw std::runtime_error("No devices are suitable");
		}
		
		uint32_t findQueueFamilyIndex(vk::PhysicalDevice physicalDevice, vk::QueueFlags queueType)
		{
			std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

			for (uint32_t i = 0 ; i < queueFamilies.size() ; i++)
			{
				if ((queueFamilies[i].queueCount > 0) && (queueFamilies[i].queueFlags & queueType))
					return i;
			}

			throw std::runtime_error("Could't find a suitable queue family");
		}
		
		vk::Device createLogicalDevice(vk::PhysicalDevice physicalDevice)
		{
			uint32_t graphicsQueueFamilyIndex = findQueueFamilyIndex(physicalDevice,
					vk::QueueFlagBits::eGraphics);

			float queuePriority = 1.0f;
			vk::DeviceQueueCreateInfo queueCreateInfo(
					vk::DeviceQueueCreateFlags(), graphicsQueueFamilyIndex, 1, &queuePriority
					);

			vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

			uint32_t enabledLayerCount = 0;
			const char** ppEnabledLayers = nullptr;
			if (Logger::isEnabled) {
				enabledLayerCount = 1;
				ppEnabledLayers = (const char**)malloc(sizeof(const char*));
				ppEnabledLayers[0] = "VK_LAYER_KHRONOS_validation";
			}

			uint32_t enabledExtensionCount = 1;
			const char** ppEnabledExtensions = (const char**)malloc(enabledExtensionCount * sizeof(char*));
			uint32_t offset = 0;
			ppEnabledExtensions[offset++] = "VK_KHR_portability_subset";
	
			vk::DeviceCreateInfo(
					vk::DeviceCreateFlags(),
					1, &queueCreateInfo,
					enabledLayerCount, ppEnabledLayers,
					enabledExtensionCount, ppEnabledExtensions,
					&deviceFeatures
					);
		}
		
	} /* device */ 
	
} /* dg */ 
