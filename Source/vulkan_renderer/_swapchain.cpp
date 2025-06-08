#include "vulkan_renderer/_swapchain.hpp"

namespace dg
{
  void SwapChain::createSwapChain()
  {
    SwapChainSupportDetails swapChainSupport = m_toolBox.swapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
    m_extent = chooseExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo(
        {}, m_toolBox.surface, imageCount, surfaceFormat.format,
        surfaceFormat.colorSpace, m_extent, 1,
        vk::ImageUsageFlagBits::eColorAttachment,
        {}, // assigned later
        {}, // assigned later
        swapChainSupport.capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, vk::True,
        m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->m_swapChain
        );

    QueueFamilyIndices indices = m_toolBox.queueFamilyIndices;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    m_swapChain =	m_toolBox.device.createSwapchainKHR(createInfo);

    m_images = m_toolBox.device.getSwapchainImagesKHR(m_swapChain);
    m_format = surfaceFormat.format;

    imageCount = m_images.size();
  }

	vk::SurfaceFormatKHR SwapChain::chooseSurfaceFormat(
			const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		for (const auto &availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8Unorm &&
					availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		}

		return availableFormats[0];
	}

  // TODO : give a vector of modes, take the first available
	vk::PresentModeKHR SwapChain::choosePresentMode(
			const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		for (const auto &availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
        PLOG_INFO << "Present mode : Mailbox";
				return availablePresentMode;
			}
			/*else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
        PLOG_INFO << "Present mode : Immediate";
				return availablePresentMode;
			}*/
		}

    PLOG_INFO << "Present mode : V-SYNC (Fifo)";
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		} else
		{
			vk::Extent2D actualExtent = m_windowExtent;
			actualExtent.width = std::max(
					capabilities.minImageExtent.width,
					std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(
					capabilities.minImageExtent.height,
					std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void SwapChain::createImageViews()
	{
		m_imageViews.resize(imageCount);
		for (size_t i = 0; i < imageCount; i++)
		{
			vk::ImageViewCreateInfo viewInfo(
					{}, m_images[i], vk::ImageViewType::e2D, m_format, {},
					vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
					);

			m_imageViews[i] = m_toolBox.device.createImageView(viewInfo);
		}
	}

	void SwapChain::createDepthResources()
	{
		vk::Format depthFormat = findDepthFormat();

		m_depthImages.resize(imageCount);
		m_depthImageAllocations.resize(imageCount);
		m_depthImageViews.resize(imageCount);

		for (int i = 0; i < m_depthImages.size(); i++)
		{
			vk::ImageCreateInfo imageInfo(
					{}, vk::ImageType::e2D, depthFormat,
					{ m_extent.width, m_extent.height, 1 },
					1, 1,
					vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
					vk::ImageUsageFlagBits::eDepthStencilAttachment,
					vk::SharingMode::eExclusive, {}
					);
			
			auto imageHandle = m_toolBox.allocator.createImage(imageInfo, {{}, vma::MemoryUsage::eAuto});
			m_depthImages[i] = imageHandle.first; 
			m_depthImageAllocations[i] = imageHandle.second; 

			vk::ImageViewCreateInfo viewInfo(
					{}, m_depthImages[i], vk::ImageViewType::e2D, depthFormat, {},
					{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}
					);

			m_depthImageViews[i] = m_toolBox.device.createImageView(viewInfo);
		}
	}

	vk::Format SwapChain::findDepthFormat()
  {
		return m_toolBox.findSupportedFormat(
				{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
				vk::ImageTiling::eOptimal,
				vk::FormatFeatureFlagBits::eDepthStencilAttachment);
  }
}
