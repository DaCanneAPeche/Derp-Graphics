#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "core/application_info.hpp"
#include "dg_window.hpp"
#include "_vulkan/structs.hpp"

namespace dg
{
  class VulkanToolBox
  {
    public:
      void init(const ApplicationInfo& applicationInfo, Window& window);
      ~VulkanToolBox() {};

      vk::Instance instance;
      vk::PhysicalDevice physicalDevice;
      vk::Device device;
      vk::SurfaceKHR surface;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;
			vk::CommandPool commandPool;
      SwapChainSupportDetails swapChainSupport;
      QueueFamilyIndices physicalDeviceQueueFamilyIndices;

			[[nodiscard]] vk::CommandBuffer beginSingleTimeCommands() const; 
			void endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const;

      [[nodiscard]] vk::Format findSupportedFormat(
				const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
        vk::FormatFeatureFlags features) const;

    private:
      [[nodiscard]] std::vector<const char*> getRequestedExtensions() const;

      std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
      std::vector<const char*> m_deviceExtensions = {vk::KHRSwapchainExtensionName};

#ifdef NDEBUG
      const bool m_enableValidationLayers = false;
#else
      const bool m_enableValidationLayers = true;
#endif
      [[nodiscard]] bool areValidationLayersSupported() const;
  };
}
