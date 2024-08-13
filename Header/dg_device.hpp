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
		bool isComplete()
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

			[[nodiscard]] SwapChainSupportDetails getSwapChainSupport()
			{ 
				return querySwapChainSupport(physical);
			};

			[[nodiscard]] vk::SurfaceKHR& surface() { return m_surface; }
			[[nodiscard]] QueueFamilyIndices physicalDeviceQueueFamilyIndices()
			{
				return findQueueFamilyIndices(physical);
			}

			vk::Format findSupportedFormat(
				const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

			vk::PhysicalDevice physical;
			vk::Device device;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;

		private:
			void pickPhysicalDevice();
			void createLogicalDevice();
			void createWindowSurface();
			[[nodiscard]] bool isDeviceSuitable(vk::PhysicalDevice physicalDevice);
			[[nodiscard]] bool areExtensionsSupportedBy(vk::PhysicalDevice physicalDevice);
			[[nodiscard]] uint32_t findQueueFamilyIndex(vk::QueueFlags queueType);
			[[nodiscard]] QueueFamilyIndices findQueueFamilyIndices(vk::PhysicalDevice physicalDevice);
			[[nodiscard]] SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice physicalDevice);

			const std::vector<const char*>& m_extensions;
			vk::Instance& m_instance;
			Window& m_window; 
			vk::SurfaceKHR m_surface;
	};
	
} /* dg */ 
