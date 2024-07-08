#pragma once

#include "vulkan/vulkan.hpp"

#include <span>
#include <array>

namespace dg
{

	class Device
	{

		public:
			Device(vk::Instance& instance, const std::vector<const char*>& extensions);
			~Device();

			Device(const Device&) = delete;
			Device& operator=(const Device&) = delete;

			vk::PhysicalDevice physical;
			vk::Device device;

			void init()
			{
				pickPhysicalDevice();
				createLogicalDevice();
			}

			void clean()
			{
				device.destroy();
			}

		private:
			void pickPhysicalDevice();
			void createLogicalDevice();
			bool areExtensionsSupportedBy(const vk::PhysicalDevice& physicalDevice);
			uint32_t findQueueFamilyIndex(vk::QueueFlags queueType);

			const std::vector<const char*>& m_extensions;
			vk::Instance& m_instance;
		
	};
	
} /* dg */ 
