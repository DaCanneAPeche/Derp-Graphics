#include "dg_swapchain.hpp"
#include "dg_logger.hpp"

// std
#include <array>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace dg
{
	SwapChain::SwapChain(Device& deviceRef, vk::Extent2D extent)
			: m_device {deviceRef}, m_windowExtent {extent}
	{
		
		init();
	}

	SwapChain::SwapChain(Device& deviceRef, vk::Extent2D extent, std::shared_ptr<SwapChain> previous)
			: m_device {deviceRef}, m_windowExtent {extent}, m_oldSwapChain {previous}
	{
		init();

		m_oldSwapChain = nullptr;
	}

	void SwapChain::init()
	{
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDepthResources();
		createFramebuffers();
		createSyncObjects();
	}

	SwapChain::~SwapChain()
	{
		for (auto& imageView : m_swapChainImageViews)
			m_device.device.destroyImageView(imageView);
		m_swapChainImageViews.clear();

		if (m_swapChain != nullptr)
		{
			m_device.device.destroySwapchainKHR(m_swapChain);
			m_swapChain = nullptr;
		}

		for (size_t i = 0; i < m_depthImages.size(); i++)
		{
			m_device.device.destroyImageView(m_depthImageViews[i]);
			MemoryAllocator::destroyImage(m_depthImages[i], m_depthImageMemorys[i]);
		}

		for (auto framebuffer : m_swapChainFramebuffers)
			m_device.device.destroyFramebuffer(framebuffer);

		m_device.device.destroyRenderPass(m_renderPass);

		// cleanup synchronization objects
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_device.device.destroySemaphore(m_renderFinishedSemaphores[i]);
			m_device.device.destroySemaphore(m_imageAvailableSemaphores[i]);
			m_device.device.destroyFence(m_inFlightFences[i]);
		}
	}

	vk::Result SwapChain::acquireNextImage(uint32_t& imageIndex)
	{
		if (m_device.device.waitForFences(
				m_inFlightFences[m_currentFrame],
				vk::True,
				std::numeric_limits<uint64_t>::max()
				) == vk::Result::eTimeout)
		{
			// Shouldn't happend because of the very hight timeout, but I didn't like the [[nodiscard]] warning
			// Should maybe be an error I guess
			Logger::msgLn("Warning (SwapChain::acquireNextImage) : Timeout while waiting for fences");
		}

		auto handle = m_device.device.acquireNextImageKHR(
				m_swapChain,
				std::numeric_limits<uint64_t>::max(),
				m_imageAvailableSemaphores[m_currentFrame]
				);

		imageIndex = handle.value;
		return handle.result;
	}

	vk::Result SwapChain::submitCommandBuffers(
			const vk::CommandBuffer& buffers, uint32_t& imageIndex)
	{
		if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
		{
			auto _ = m_device.device.waitForFences(
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

		m_device.device.resetFences(m_inFlightFences[m_currentFrame]);
		m_device.graphicsQueue.submit(submitInfo, m_inFlightFences[m_currentFrame]);

		vk::SwapchainKHR swapChains[] = {m_swapChain};

		vk::PresentInfoKHR presentInfo(signalSemaphores, swapChains, imageIndex);

		auto result = m_device.presentQueue.presentKHR(presentInfo);

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}

	void SwapChain::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = m_device.getSwapChainSupport();

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
				imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo(
				{},
				m_device.surface(),
				imageCount,
				surfaceFormat.format,
				surfaceFormat.colorSpace,
				extent,
				1,
				vk::ImageUsageFlagBits::eColorAttachment,
				{}, // assigned later
				{}, // assigned later
				swapChainSupport.capabilities.currentTransform,
				vk::CompositeAlphaFlagBitsKHR::eOpaque,
				presentMode,
				vk::True,
				m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->m_swapChain
				);

		QueueFamilyIndices indices = m_device.physicalDeviceQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		m_swapChain =	m_device.device.createSwapchainKHR(createInfo);

		m_swapChainImages = m_device.device.getSwapchainImagesKHR(m_swapChain);
		m_swapChainImageFormat = surfaceFormat.format;
		m_swapChainExtent = extent;
	}

	void SwapChain::createImageViews()
	{
		m_swapChainImageViews.resize(m_swapChainImages.size());
		for (size_t i = 0; i < m_swapChainImages.size(); i++)
		{
			vk::ImageViewCreateInfo viewInfo(
					{},
					m_swapChainImages[i],
					vk::ImageViewType::e2D,
					m_swapChainImageFormat,
					{},
					vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
					);

			m_swapChainImageViews[i] = m_device.device.createImageView(viewInfo);
		}
	}

	void SwapChain::createRenderPass()
	{
		vk::AttachmentDescription depthAttachment(
				{},
				findDepthFormat(),
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eDontCare,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eDepthStencilAttachmentOptimal
				);

		vk::AttachmentReference depthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::AttachmentDescription colorAttachment(
				{},
				getSwapChainImageFormat(),
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::ePresentSrcKHR
				);

		vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

		vk::SubpassDescription subpass(
				{},
				vk::PipelineBindPoint::eGraphics,
				{},
				colorAttachmentRef,
				{},
				&depthAttachmentRef,
				{}
				);

		vk::SubpassDependency dependency(
				vk::SubpassExternal,
				0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
				{},
				vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
				{}
				);

		std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
		vk::RenderPassCreateInfo renderPassInfo(
				{},
				attachments,
				subpass,
				dependency
				);

		m_renderPass = m_device.device.createRenderPass(renderPassInfo);
	}

	void SwapChain::createFramebuffers()
	{
		m_swapChainFramebuffers.resize(imageCount());
		for (size_t i = 0; i < imageCount(); i++)
		{
			std::array<vk::ImageView, 2> attachments = {m_swapChainImageViews[i], m_depthImageViews[i]};

			vk::Extent2D swapChainExtent = getSwapChainExtent();
			vk::FramebufferCreateInfo framebufferInfo(
					{},
					m_renderPass,
					attachments,
					swapChainExtent.width,
					swapChainExtent.height,
					1
					);
			
			m_swapChainFramebuffers[i] = m_device.device.createFramebuffer(framebufferInfo);
		}
	}

	void SwapChain::createDepthResources()
	{
		vk::Format depthFormat = findDepthFormat();
		vk::Extent2D swapChainExtent = getSwapChainExtent();

		m_depthImages.resize(imageCount());
		m_depthImageMemorys.resize(imageCount());
		m_depthImageViews.resize(imageCount());

		for (int i = 0; i < m_depthImages.size(); i++)
		{
			vk::ImageCreateInfo imageInfo(
					{},
					vk::ImageType::e2D,
					depthFormat,
					{
						swapChainExtent.width,
						swapChainExtent.height,
						1
					},
					1,
					1,
					vk::SampleCountFlagBits::e1,
					vk::ImageTiling::eOptimal,
					vk::ImageUsageFlagBits::eDepthStencilAttachment,
					vk::SharingMode::eExclusive,
					{}
					);
			
			auto imageHandle = MemoryAllocator::createImage(imageInfo, {{}, vma::MemoryUsage::eAuto});
			m_depthImages[i] = imageHandle.first; 
			m_depthImageMemorys[i] = imageHandle.second; 

			vk::ImageViewCreateInfo viewInfo(
					{},
					m_depthImages[i],
					vk::ImageViewType::e2D,
					depthFormat,
					{},
					{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}
					);

			m_depthImageViews[i] = m_device.device.createImageView(viewInfo);
		}
	}

	void SwapChain::createSyncObjects()
	{
		m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_imagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

		vk::SemaphoreCreateInfo semaphoreInfo;

		vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_imageAvailableSemaphores[i] = m_device.device.createSemaphore(semaphoreInfo);
			m_renderFinishedSemaphores[i] = m_device.device.createSemaphore(semaphoreInfo);
			m_inFlightFences[i] = m_device.device.createFence(fenceInfo);
		}
	}

	vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
			const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		for (const auto &availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8Srgb &&
					availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR SwapChain::chooseSwapPresentMode(
			const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		for (const auto &availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				Logger::msgLn("Present mode: Mailbox");
				return availablePresentMode;
			}
			/*else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				Logger::msgLn("Present mode: Immediate");
				return availablePresentMode;
			}*/
		}

		Logger::msgLn("Present mode: V-Sync");
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
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

	vk::Format SwapChain::findDepthFormat() {
		return m_device.findSupportedFormat(
				{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
				vk::ImageTiling::eOptimal,
				vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}

} /* dg */ 
