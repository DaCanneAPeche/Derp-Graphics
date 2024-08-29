#pragma once

#include "dg_device.hpp"
#include "dg_globals.hpp"

// vulkan headers
#include <vulkan/vulkan.hpp>

// std
#include <memory>
#include <string>
#include <vector>

namespace dg
{
	class SwapChain {
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device &deviceRef, vk::Extent2D windowExtent);
		SwapChain(Device &deviceRef, vk::Extent2D windowExtent, std::shared_ptr<SwapChain> previous);
		~SwapChain();

		SwapChain(const SwapChain &) = delete;
		SwapChain& operator=(const SwapChain &) = delete;

		vk::Framebuffer getFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
		vk::RenderPass getRenderPass() { return m_renderPass; }
		vk::ImageView getImageView(int index) { return m_swapChainImageViews[index]; }
		size_t imageCount() { return m_swapChainImages.size(); }
		vk::Format getSwapChainImageFormat() { return m_swapChainImageFormat; }
		vk::Extent2D getSwapChainExtent() { return m_swapChainExtent; }
		uint32_t width() { return m_swapChainExtent.width; }
		uint32_t height() { return m_swapChainExtent.height; }

		float extentAspectRatio() {
			return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
		}
		vk::Format findDepthFormat();

		vk::Result acquireNextImage(uint32_t& imageIndex);
		vk::Result submitCommandBuffers(const vk::CommandBuffer& buffers, uint32_t& imageIndex);

	private:
		void init();
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();

		// Helper functions
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR> &availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(
		const std::vector<vk::PresentModeKHR> &availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

		vk::Format m_swapChainImageFormat;
		vk::Extent2D m_swapChainExtent;

		std::vector<vk::Framebuffer> m_swapChainFramebuffers;
		vk::RenderPass m_renderPass;

		std::vector<vk::Image> m_depthImages;
		std::vector<vma::Allocation> m_depthImageAllocations;
		std::vector<vk::ImageView> m_depthImageViews;
		std::vector<vk::Image> m_swapChainImages;
		std::vector<vk::ImageView> m_swapChainImageViews;

		Device& m_device;
		vk::Extent2D m_windowExtent;

		vk::SwapchainKHR m_swapChain;
		std::shared_ptr<SwapChain> m_oldSwapChain;

		std::vector<vk::Semaphore> m_imageAvailableSemaphores;
		std::vector<vk::Semaphore> m_renderFinishedSemaphores;
		std::vector<vk::Fence> m_inFlightFences;
		std::vector<vk::Fence> m_imagesInFlight;
		size_t m_currentFrame = 0;
	};

} /* dg */
