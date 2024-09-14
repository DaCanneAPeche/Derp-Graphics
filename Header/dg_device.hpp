#pragma once

#include "vulkan/vulkan.hpp"

#include "dg_window.hpp"

#include <span>
#include <array>
#include <optional> 

namespace dg
{
	
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	class Device
	{

		public:
			Device(vk::Instance& instance, const std::vector<const char*>& extensions, Window& window);

			Device(const Device&) = delete;
			Device& operator=(const Device&) = delete;

			void init();
			void clean();

			[[nodiscard]] SwapChainSupportDetails getSwapChainSupport() const
			{ 
				return querySwapChainSupport(physical);
			};

			[[nodiscard]] vk::SurfaceKHR& surface() { return m_surface; }
			[[nodiscard]] QueueFamilyIndices physicalDeviceQueueFamilyIndices() const
			{
				return findQueueFamilyIndices(physical);
			}

			[[nodiscard]] vk::Format findSupportedFormat(
				const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;

			vk::PhysicalDevice physical;
			vk::Device device;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;
			vk::CommandPool commandPool;

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
			vk::Instance& m_instance;
			Window& m_window; 
			vk::SurfaceKHR m_surface;
	};
	
} /* dg */ 
