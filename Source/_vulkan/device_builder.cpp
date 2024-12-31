#include "_vulkan/device_builder.hpp"
#include "_vulkan/buffer.hpp"

// std
#include <stdexcept>
#include <set>

#include <plog/Log.h>

namespace dg
{

	DeviceBuilder::DeviceBuilder(VulkanToolBox& toolBox,
      const std::vector<const char*>& extensions, Window& window)
		: m_toolBox(toolBox), m_extensions(extensions), m_window(window)
	{
		createWindowSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createCommandPool();
	}

  void DeviceBuilder::pickPhysicalDevice()
	{
		std::vector<vk::PhysicalDevice> availableDevices = m_toolBox.instance.enumeratePhysicalDevices();

		for (const auto& device : availableDevices)
		{
			if (isDeviceSuitable(device))
			{
				physical = device;
        PLOG_INFO << "Physical device correctly choosen : "
          << device.getProperties().deviceName;
				return;
			}
		}

    PLOG_FATAL << "No devices are suitable";
		throw std::runtime_error("No devices are suitable");
	}
	
  void DeviceBuilder::createLogicalDevice()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilyIndices(physical);

		std::set<uint32_t> uniqueQueueFamilies = {
			queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value()
		};
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

		vk::PhysicalDeviceFeatures features;
		features.samplerAnisotropy = vk::True;
    features.fillModeNonSolid = vk::True;

    vk::PhysicalDeviceRobustness2FeaturesEXT robustnessFeatures(vk::False,
        vk::False, vk::True);

    vk::PhysicalDeviceVulkan12Features vk12Features;
    vk12Features.pNext = &robustnessFeatures;
    vk12Features.runtimeDescriptorArray = vk::True;
    vk12Features.descriptorBindingPartiallyBound = vk::True;

    vk::PhysicalDeviceFeatures2 deviceFeatures(features,
        &vk12Features); 

		vk::DeviceCreateInfo deviceInfo(
        {}, queueCreateInfos,
				{}, // enabledLayers, deprecated and ignored (see doc)
				m_extensions, nullptr, &deviceFeatures
				);

		device = physical.createDevice(deviceInfo);
    PLOG_INFO << "Logical device created";

		graphicsQueue = device.getQueue(queueFamilyIndices.graphicsFamily.value(), 0);
		presentQueue = device.getQueue(queueFamilyIndices.presentFamily.value(), 0);
	}

	bool DeviceBuilder::isDeviceSuitable(vk::PhysicalDevice physicalDevice) const
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
					 supportedFeatures.samplerAnisotropy &&
           supportedFeatures.fillModeNonSolid;

	}

	bool DeviceBuilder::areExtensionsSupportedBy(vk::PhysicalDevice physicalDevice) const
	{
		std::vector<vk::ExtensionProperties> availableExtensions =
			physicalDevice.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(m_extensions.begin(), m_extensions.end());

		for (const auto &extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	uint32_t DeviceBuilder::findQueueFamilyIndex(vk::QueueFlags queueType) const
	{
		std::vector<vk::QueueFamilyProperties> queueFamilies = physical.getQueueFamilyProperties();

		for (uint32_t i = 0 ; i < queueFamilies.size() ; i++)
		{
			if ((queueFamilies[i].queueCount > 0) && (queueFamilies[i].queueFlags & queueType))
				return i;
		}

		throw std::runtime_error("Could't find a suitable queue family");
	}
	
	QueueFamilyIndices DeviceBuilder::findQueueFamilyIndices(vk::PhysicalDevice physicalDevice) const
	{
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) 
			{
				indices.graphicsFamily = i;
			}

			vk::Bool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, surface);
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
	
	SwapChainSupportDetails DeviceBuilder::querySwapChainSupport(vk::PhysicalDevice physicalDevice) const
	{
		return {
			physicalDevice.getSurfaceCapabilitiesKHR(surface),
			physicalDevice.getSurfaceFormatsKHR(surface),
			physicalDevice.getSurfacePresentModesKHR(surface)
		};
	}
	
	void DeviceBuilder::createWindowSurface()
	{
		// GLFW takes a C surface, which is then casted to a C++ surface
		VkSurfaceKHR cSurface;
		m_window.createWindowSurface(m_toolBox.instance, &cSurface);
		surface = static_cast<vk::SurfaceKHR>(cSurface);
	}

	void DeviceBuilder::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilyIndices(physical);
		vk::CommandPoolCreateInfo commandPoolInfo({
				vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer
				}, queueFamilyIndices.graphicsFamily.value());
		commandPool = device.createCommandPool(commandPoolInfo);
	}
		
} /* dg */ 
