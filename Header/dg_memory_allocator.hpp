#pragma once

#define VMA_IMPLEMENTATION
#define VA_VULKAN_VERSION 1003000
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

namespace dg
{

	// Singleton 
	class MemoryAllocator
	{
	public:

		MemoryAllocator(const MemoryAllocator&) = delete;
		void operator=(const MemoryAllocator&) = delete;

		static MemoryAllocator& get() {
			static MemoryAllocator instance;
			return instance;
		}

		static void init(const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
				const vk::Instance& instance)
		{
			get().iInit(physicalDevice, device, instance);
		}

		static VmaAllocator& getAllocator() { return get().m_allocator; }

	private:
		MemoryAllocator() {}

		~MemoryAllocator()
		{
			vmaDestroyAllocator(m_allocator);
		}

		void iInit(const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
				const vk::Instance& instance)
		{
			VmaVulkanFunctions vulkanFunctions = {};
			vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
			vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

			VmaAllocatorCreateInfo allocatorCreateInfo = {};
			allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
			allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			allocatorCreateInfo.physicalDevice = physicalDevice;
			allocatorCreateInfo.device = device;
			allocatorCreateInfo.instance = instance;
			allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

			vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);
		}

		VmaAllocator m_allocator;

	};

} /* dg */ 
