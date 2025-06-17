#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan_renderer/render_pass.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "_vulkan/structs.hpp"

#include "plog/Log.h"

#include <memory>
#include <vector>

namespace dg
{
  class SwapChain
  {
    public:
      static constexpr uint8_t MAX_FRAMES_IN_FLIGHT = 2;

      SwapChain(VulkanToolBox& toolBox, const vk::Extent2D& windowExtent,
          RenderPass& renderPass)
        : m_toolBox(toolBox), m_windowExtent(windowExtent)
      {
        create(renderPass);
      }

      SwapChain(VulkanToolBox& toolBox, const vk::Extent2D& windowExtent,
          RenderPass& renderPass, std::shared_ptr<SwapChain> previous)
        : m_toolBox(toolBox), m_windowExtent(windowExtent),
        m_oldSwapChain(previous)
    {
      create(renderPass);
      m_oldSwapChain = nullptr;
    }

      void create(RenderPass& renderPass)
      {
        createSwapChain();
        createImageViews();
        createDepthResources();
        createFramebuffers(renderPass);
        createSyncObjects();
      }

      vk::Framebuffer getFrameBuffer(size_t index)
      {
        return m_framebuffers[index];
      }

      vk::Result acquireNextImage(uint32_t& imageIndex);
      vk::Result submitCommandBuffers(const vk::CommandBuffer& buffers, uint32_t& imageIndex);

      uint32_t imageCount = 0;
      vk::Extent2D extent;

    private:
      void createSwapChain();
      void createImageViews();
      void createDepthResources();
      void createFramebuffers(RenderPass& renderPass);
      void createSyncObjects();

      // Helper functions needed to create the swap chain
      vk::SurfaceFormatKHR chooseSurfaceFormat(
          const std::vector<vk::SurfaceFormatKHR> &availableFormats);
      vk::PresentModeKHR choosePresentMode(
          const std::vector<vk::PresentModeKHR> &availablePresentModes);
      vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

      VulkanToolBox& m_toolBox;

      vk::SwapchainKHR m_swapChain;
      std::shared_ptr<SwapChain> m_oldSwapChain;

      vk::Format m_format;
      vk::Extent2D m_windowExtent;

      std::vector<vk::Image> m_images;
      std::vector<vk::ImageView> m_imageViews;

      std::vector<vk::Image> m_depthImages;
      std::vector<vma::Allocation> m_depthImageAllocations;
      std::vector<vk::ImageView> m_depthImageViews;

      std::vector<vk::Framebuffer> m_framebuffers;

      std::vector<vk::Semaphore> m_imageAvailableSemaphores;
      std::vector<vk::Semaphore> m_renderFinishedSemaphores;
      std::vector<vk::Fence> m_inFlightFences;
      std::vector<vk::Fence> m_imagesInFlight;
      size_t m_currentFrame = 0;

  };
}
