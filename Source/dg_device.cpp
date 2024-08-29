#include "dg_logger.hpp"
#include "dg_device.hpp"
#include "dg_globals.hpp"

// std
#include <stdexcept>
#include <set>

namespace dg
{

	Device::Device(vk::Instance& instance, const std::vector<const char*>& extensions, Window& window)
		: m_instance(instance), m_extensions(extensions), m_window(window)
	{
	}

	void Device::init()
	{
		createWindowSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createCommandPool();
	}

	void Device::clean()
	{
		device.destroyCommandPool(commandPool);
		device.destroy();
    m_instance.destroySurfaceKHR(m_surface);
	}

	void Device::pickPhysicalDevice()
	{
		std::vector<vk::PhysicalDevice> availableDevices = m_instance.enumeratePhysicalDevices();

		for (const auto& device : availableDevices)
		{
			if (isDeviceSuitable(device))
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
		QueueFamilyIndices queueFamilyIndices = findQueueFamilyIndices(physical);

		std::set<uint32_t> uniqueQueueFamilies = {
			queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value()
		};
		std::cout << uniqueQueueFamilies.size() << std::endl;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(0);

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo(
					{},
					queueFamily,
					1,
					&queuePriority,
					nullptr
					);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures;

		vk::DeviceCreateInfo deviceInfo(
				vk::DeviceCreateFlags(),
				queueCreateInfos,
				{}, // enabledLayers, deprecated and ignored (see doc)
				m_extensions,
				&deviceFeatures
				);

		device = physical.createDevice(deviceInfo);
		Logger::msgLn("Logical device created");

		graphicsQueue = device.getQueue(queueFamilyIndices.graphicsFamily.value(), 0);
		presentQueue = device.getQueue(queueFamilyIndices.presentFamily.value(), 0);
	}

	bool Device::isDeviceSuitable(vk::PhysicalDevice physicalDevice)
	{
		QueueFamilyIndices indices = findQueueFamilyIndices(physicalDevice);
		bool extensionsSupported = areExtensionsSupportedBy(physicalDevice);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		vk::PhysicalDeviceFeatures supportedFeatures = physicalDevice.getFeatures();

		return indices.isComplete() && extensionsSupported && swapChainAdequate &&
					 supportedFeatures.samplerAnisotropy;

	}

	bool Device::areExtensionsSupportedBy(vk::PhysicalDevice physicalDevice)
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
	
	QueueFamilyIndices Device::findQueueFamilyIndices(vk::PhysicalDevice physicalDevice)
	{
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) 
			{
				indices.graphicsFamily = i;
			}

			vk::Bool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, m_surface);
			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
			}
			
			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;

	}
	
	SwapChainSupportDetails Device::querySwapChainSupport(vk::PhysicalDevice physicalDevice)
	{
		return {
			physicalDevice.getSurfaceCapabilitiesKHR(m_surface),
			physicalDevice.getSurfaceFormatsKHR(m_surface),
			physicalDevice.getSurfacePresentModesKHR(m_surface)
		};
	}
	
	vk::Format Device::findSupportedFormat(
		const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
	{
		for (vk::Format format : candidates) {
			vk::FormatProperties props;
			props = physical.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
			{
				return format;
			} else if (
					tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}
	
	void Device::createWindowSurface()
	{
		// GLFW takes a C surface, which is then casted to a C++ surface
		VkSurfaceKHR cSurface;
		m_window.createWindowSurface(m_instance, &cSurface);
		m_surface = static_cast<vk::SurfaceKHR>(cSurface);
	}

	void Device::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilyIndices(physical);
		vk::CommandPoolCreateInfo commandPoolInfo({
				vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer
				}, queueFamilyIndices.graphicsFamily.value());
		commandPool = device.createCommandPool(commandPoolInfo);
	}
	
} /* dg */ 
