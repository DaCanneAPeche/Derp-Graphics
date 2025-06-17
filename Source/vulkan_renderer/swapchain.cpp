#include "vulkan_renderer/swapchain.hpp"

namespace dg
{

  SwapChain::~SwapChain()
  {
    for (auto& imageView : m_imageViews) m_toolBox.device.destroyImageView(imageView);
		m_imageViews.clear();

    m_toolBox.device.destroySwapchainKHR(m_swapChain);

    for (size_t i = 0; i < m_depthImages.size(); i++)
		{
			m_toolBox.device.destroyImageView(m_depthImageViews[i]);
			m_toolBox.allocator.destroyImage(m_depthImages[i], m_depthImageAllocations[i]);
		}

    for (auto& framebuffer : m_framebuffers)
			m_toolBox.device.destroyFramebuffer(framebuffer);

    // cleanup synchronization objects
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_toolBox.device.destroySemaphore(m_renderFinishedSemaphores[i]);
			m_toolBox.device.destroySemaphore(m_imageAvailableSemaphores[i]);
			m_toolBox.device.destroyFence(m_inFlightFences[i]);
		}
  }

	vk::Result SwapChain::acquireNextImage(uint32_t& imageIndex)
	{
		if (m_toolBox.device.waitForFences( m_inFlightFences[m_currentFrame],
				vk::True, std::numeric_limits<uint64_t>::max()) == vk::Result::eTimeout)
		{
			// Shouldn't appear because of the very hight timeout,
      // but I didn't like the [[nodiscard]] warning
      PLOG_ERROR << "Timeout while waiting for fences";
		}

		auto handle = m_toolBox.device.acquireNextImageKHR(
				m_swapChain,
				std::numeric_limits<uint64_t>::max(),
				m_imageAvailableSemaphores[m_currentFrame]
				);

		imageIndex = handle.value;
		return handle.result;
	}

	vk::Result SwapChain::submitCommandBuffers(const vk::CommandBuffer& buffers,
      uint32_t& imageIndex)
	{
		if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
		{
			auto _ = m_toolBox.device.waitForFences(
					m_imagesInFlight[imageIndex],
					vk::True,
					std::numeric_limits<uint64_t>::max()
					);
		}

		m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];
		
		vk::Semaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::Semaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};

		vk::SubmitInfo submitInfo(waitSemaphores, waitStages, buffers, signalSemaphores);

		m_toolBox.device.resetFences(m_inFlightFences[m_currentFrame]);
		m_toolBox.graphicsQueue.submit(submitInfo, m_inFlightFences[m_currentFrame]);

		vk::SwapchainKHR swapChains[] = {m_swapChain};

		vk::PresentInfoKHR presentInfo(signalSemaphores, swapChains, imageIndex);

		auto result = m_toolBox.presentQueue.presentKHR(presentInfo);

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}

  void SwapChain::createSwapChain()
  {
    SwapChainSupportDetails swapChainSupport = m_toolBox.swapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = m_toolBox.getSwapChainSurfaceFormat();
    vk::PresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
    extent = chooseExtent(swapChainSupport.capabilities);

    uint32_t minImageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        minImageCount > swapChainSupport.capabilities.maxImageCount)
    {
      minImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo(
        {}, m_toolBox.surface, minImageCount, surfaceFormat.format,
        surfaceFormat.colorSpace, extent, 1,
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
		vk::Format depthFormat = m_toolBox.findDepthFormat();

		m_depthImages.resize(imageCount);
		m_depthImageAllocations.resize(imageCount);
		m_depthImageViews.resize(imageCount);

		for (int i = 0; i < m_depthImages.size(); i++)
		{
			vk::ImageCreateInfo imageInfo(
					{}, vk::ImageType::e2D, depthFormat,
					{ extent.width, extent.height, 1 },
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

	void SwapChain::createFramebuffers(RenderPass& renderPass)
	{
		m_framebuffers.resize(imageCount);
		for (size_t i = 0; i < imageCount; i++)
		{
			std::array<vk::ImageView, 2> attachments = {m_imageViews[i], m_depthImageViews[i]};

			vk::Extent2D swapChainExtent = extent;
			vk::FramebufferCreateInfo framebufferInfo(
					{},
					renderPass.renderPass,
					attachments,
					swapChainExtent.width,
					swapChainExtent.height,
					1
					);
			
			m_framebuffers[i] = m_toolBox.device.createFramebuffer(framebufferInfo);
		}
	}

	void SwapChain::createSyncObjects()
	{
		m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_imagesInFlight.resize(imageCount, VK_NULL_HANDLE);

		vk::SemaphoreCreateInfo semaphoreInfo;

		vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_imageAvailableSemaphores[i] = m_toolBox.device.createSemaphore(semaphoreInfo);
			m_renderFinishedSemaphores[i] = m_toolBox.device.createSemaphore(semaphoreInfo);
			m_inFlightFences[i] = m_toolBox.device.createFence(fenceInfo);
		}
	}
}
