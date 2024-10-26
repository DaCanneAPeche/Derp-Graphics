#include "_vulkan/vulkan_tool_box.hpp"
#include "_vulkan/instance.hpp"
#include "_vulkan/device_builder.hpp"
#include "_vulkan/debug_messenger.hpp"

namespace dg
{

  void VulkanToolBox::init(const ApplicationInfo& applicationInfo,
      Window& window)
  {
    vk::ApplicationInfo appInfo(
        applicationInfo.name.c_str(),
        VK_MAKE_VERSION(applicationInfo.version[0], 
          applicationInfo.version[1], applicationInfo.version[2]),
        "Derp Graphics",
        VK_MAKE_VERSION(1, 0, 0),
        vk::ApiVersion13
        );

    instance = createInstance(getRequestedExtensions(), m_validationLayers,
        appInfo);

    m_dispatchLoader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
    m_debugMessenger = createDebugMessenger(instance, m_dispatchLoader);

    DeviceBuilder deviceBuilder(*this, m_deviceExtensions, window);

    device = deviceBuilder.device;
    physicalDevice = deviceBuilder.physical;
    surface = deviceBuilder.surface;
    graphicsQueue = deviceBuilder.graphicsQueue;
    presentQueue = deviceBuilder.presentQueue;
    commandPool = deviceBuilder.commandPool;

    swapChainSupport = deviceBuilder.getSwapChainSupport();
    physicalDeviceQueueFamilyIndices =
      deviceBuilder.physicalDeviceQueueFamilyIndices();

    initMemoryAllocator();
  }

  VulkanToolBox::~VulkanToolBox()
  {
    gAllocator.destroy();

		device.destroyCommandPool(commandPool);
		device.destroy();
    instance.destroySurfaceKHR(surface);

    if (m_enableValidationLayers)
      instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatchLoader);

    instance.destroy();
  }

  std::vector<const char*> VulkanToolBox::getRequestedExtensions() const
  {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers) {
      extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
  }

	vk::CommandBuffer VulkanToolBox::beginSingleTimeCommands() const
	{
		vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
		vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];
		vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.begin(beginInfo);

		return commandBuffer;
	} 

	void VulkanToolBox::endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.end();
		vk::SubmitInfo submitInfo({}, {}, commandBuffer);
		graphicsQueue.submit(submitInfo);
		graphicsQueue.waitIdle();
		device.freeCommandBuffers(commandPool, commandBuffer);
	}

  vk::Format VulkanToolBox::findSupportedFormat(
      const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
      vk::FormatFeatureFlags features) const
  {
		for (vk::Format format : candidates) {
			vk::FormatProperties props;
			props = physicalDevice.getFormatProperties(format);

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

  void VulkanToolBox::initMemoryAllocator() const
  {
    vma::VulkanFunctions vulkanFunctions(
        &vkGetInstanceProcAddr,
        &vkGetDeviceProcAddr
        );

    vma::AllocatorCreateInfo allocatorCreateInfo(
        vma::AllocatorCreateFlagBits::eExtMemoryBudget,
        physicalDevice,
        device,
        {},
        nullptr,
        nullptr,
        {},
        &vulkanFunctions,
        instance,
        vk::ApiVersion13,
        {}
        );

    gAllocator = vma::createAllocator(allocatorCreateInfo);
  }

}
