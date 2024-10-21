#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "core/application_info.hpp"
#include "dg_window.hpp"

namespace dg
{
  class VulkanToolBox
  {
    public:
      VulkanToolBox(ApplicationInfo& applicationInfo, Window& window);
      ~VulkanToolBox() {};

      vk::Instance instance;
      vk::PhysicalDevice physicalDevice;
      vk::Device device;
      vk::SurfaceKHR surface;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;
			vk::CommandPool commandPool;

			[[nodiscard]] vk::CommandBuffer beginSingleTimeCommands() const; 
			void endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const;

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
