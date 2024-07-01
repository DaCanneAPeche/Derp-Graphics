#include "vulkan/vulkan.hpp"

#include <span>

namespace dg
{

	namespace device
	{

		bool isSupporting(
				const vk::PhysicalDevice& physicalDevice,
				const std::span<char *>& requestedExtensions
				);

		bool isSuitable(const vk::PhysicalDevice physicalDevice);

		vk::PhysicalDevice choosePhysicalDevice(const vk::Instance instance);

		uint32_t findQueueFamilyIndex(vk::PhysicalDevice physicalDevice, vk::QueueFlags queueType);

		vk::Device createLogicalDevice(vk::PhysicalDevice physicalDevice);
		
	} /* device */ 
	
} /* dg */ 
