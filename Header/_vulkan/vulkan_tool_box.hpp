#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "core/application_info.hpp"
#include "vulkan_renderer/window.hpp"
#include "_vulkan/structs.hpp"

// #define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
#include "vk_mem_alloc.hpp"

namespace dg
{
  class VulkanToolBox
  {
    public:
      void init(const ApplicationInfo& applicationInfo, Window& window);
      ~VulkanToolBox();

      vk::Instance instance;
      vk::PhysicalDevice physicalDevice;
      vk::Device device;
      vk::SurfaceKHR surface;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;
			vk::CommandPool commandPool;
      vma::Allocator allocator;
      SwapChainSupportDetails swapChainSupport()
      {
        return {
          physicalDevice.getSurfaceCapabilitiesKHR(surface),
            physicalDevice.getSurfaceFormatsKHR(surface),
            physicalDevice.getSurfacePresentModesKHR(surface)
        };

      }
      QueueFamilyIndices queueFamilyIndices;

			[[nodiscard]] vk::CommandBuffer beginSingleTimeCommands() const; 
			void endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const;

      [[nodiscard]] vk::Format findSupportedFormat(
				const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
        vk::FormatFeatureFlags features) const;

      vk::Format findDepthFormat();
      vk::SurfaceFormatKHR chooseSurfaceFormat(
          const std::vector<vk::SurfaceFormatKHR>& availableFormats);
      vk::SurfaceFormatKHR getSwapChainSurfaceFormat();

    private:
      [[nodiscard]] std::vector<const char*> getRequestedExtensions() const;
      void initMemoryAllocator();

      std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
      std::vector<const char*> m_deviceExtensions = {
        vk::KHRSwapchainExtensionName,
        vk::EXTRobustness2ExtensionName
      };
      vk::DebugUtilsMessengerEXT m_debugMessenger;
      vk::DispatchLoaderDynamic m_dispatchLoader;

#ifdef NDEBUG
      const bool m_enableValidationLayers = false;
#else
      const bool m_enableValidationLayers = true;
#endif
      [[nodiscard]] bool areValidationLayersSupported() const;
  };
}
