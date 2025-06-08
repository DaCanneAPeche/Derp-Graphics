#pragma once

#include "vulkan/vulkan.hpp"
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

      SwapChain(VulkanToolBox& toolBox, const vk::Extent2D& windowExtent)
        : m_toolBox(toolBox), m_windowExtent(windowExtent) { }

      SwapChain(VulkanToolBox& toolBox, const vk::Extent2D& windowExtent,
          std::shared_ptr<SwapChain> previous) : m_toolBox(toolBox),
          m_windowExtent(windowExtent), m_oldSwapChain(previous) { }

      void create()
      {
        createSwapChain();
        createImageViews();
        createDepthResources();
        m_oldSwapChain = nullptr;
      }

      uint32_t imageCount = 0;

    private:
      void createSwapChain();
      void createImageViews();
      void createDepthResources();

      // Helper functions needed to create the swap chain
      vk::SurfaceFormatKHR chooseSurfaceFormat(
          const std::vector<vk::SurfaceFormatKHR> &availableFormats);
      vk::PresentModeKHR choosePresentMode(
          const std::vector<vk::PresentModeKHR> &availablePresentModes);
      vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

      // Helper function needed to create the depth images
      vk::Format findDepthFormat();

      VulkanToolBox& m_toolBox;

      vk::SwapchainKHR m_swapChain;
      std::shared_ptr<SwapChain> m_oldSwapChain;

      vk::Format m_format;
      vk::Extent2D m_extent;
      vk::Extent2D m_windowExtent;

      std::vector<vk::Image> m_images;
      std::vector<vk::ImageView> m_imageViews;

      std::vector<vk::Image> m_depthImages;
      std::vector<vma::Allocation> m_depthImageAllocations;
      std::vector<vk::ImageView> m_depthImageViews;

  };
}
