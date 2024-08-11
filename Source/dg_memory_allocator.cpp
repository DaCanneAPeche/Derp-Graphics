#include "dg_memory_allocator.hpp"

namespace dg
{
	
	void MemoryAllocator::iInit(const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
			const vk::Instance& instance)
	{
		vma::VulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		vma::AllocatorCreateInfo allocatorCreateInfo(
				vma::AllocatorCreateFlagBits::eExtMemoryBudget,
				physicalDevice,
				device,
				{},
				nullptr,
				nullptr,
				{},
				&vulkanFunctions,
				instance,
				vk::ApiVersion13,
				{}
				);

		m_allocator = vma::createAllocator(allocatorCreateInfo);
	}
	
	std::pair<vk::Image, vma::Allocation> MemoryAllocator::iCreateImage(const vk::ImageCreateInfo& createInfo,
			const vma::AllocationCreateInfo& allocInfo)
	{
		return m_allocator.createImage(createInfo, allocInfo);
	}

	std::pair<vk::Buffer, vma::Allocation> MemoryAllocator::iCreateBuffer(const vk::BufferCreateInfo& createInfo,
			const vma::AllocationCreateInfo& allocInfo)
	{
		return m_allocator.createBuffer(createInfo, allocInfo);
	}
	
	void MemoryAllocator::iDestroyImage(vk::Image& image, vma::Allocation& allocation)
	{
		m_allocator.destroyImage(image, allocation);
	}

	void MemoryAllocator::iDestroyBuffer(vk::Buffer& buffer, vma::Allocation& allocation)
	{
		m_allocator.destroyBuffer(buffer, allocation);
	}

} /* dg */ 
