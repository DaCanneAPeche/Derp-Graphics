#pragma once

#include "vulkan/vulkan.hpp"

#include "dg_window.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "_vulkan/structs.hpp"

#include <span>
#include <array>
#include <optional> 

namespace dg
{
	
	class DeviceBuilder
	{

		public:
			DeviceBuilder(VulkanToolBox& toolBox, 
        const std::vector<const char*>& extensions, Window& window);

			DeviceBuilder(const DeviceBuilder&) = delete;
			DeviceBuilder& operator=(const DeviceBuilder&) = delete;

			void init();
			void clean();

			[[nodiscard]] SwapChainSupportDetails getSwapChainSupport() const
			{ 
				return querySwapChainSupport(physical);
			};

			[[nodiscard]] QueueFamilyIndices physicalDeviceQueueFamilyIndices() const
			{
				return findQueueFamilyIndices(physical);
			}

			vk::PhysicalDevice physical;
			vk::Device device;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;
			vk::CommandPool commandPool;
			vk::SurfaceKHR surface;

		private:
      void pickPhysicalDevice();
      void createLogicalDevice();
			void createWindowSurface();
			void createCommandPool();
			[[nodiscard]] bool isDeviceSuitable(vk::PhysicalDevice physicalDevice) const;
			[[nodiscard]] bool areExtensionsSupportedBy(vk::PhysicalDevice physicalDevice) const;
			[[nodiscard]] uint32_t findQueueFamilyIndex(vk::QueueFlags queueType) const;
			[[nodiscard]] QueueFamilyIndices findQueueFamilyIndices(vk::PhysicalDevice physicalDevice) const;
			[[nodiscard]] SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice physicalDevice) const;

			const std::vector<const char*>& m_extensions;
      VulkanToolBox& m_toolBox;
			Window& m_window; 
	};
	
} /* dg */ 
