#pragma once

#include "vulkan/vulkan.hpp"
#include "vk_mem_alloc.hpp"

// std
#include <vector>
#include <assert.h>

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

		static vma::Allocator& getAllocator() { return get().m_allocator; }
		static std::pair<vk::Image, vma::Allocation> createImage(const vk::ImageCreateInfo& createInfo,
				const vma::AllocationCreateInfo& allocInfo)
		{
			return get().iCreateImage(createInfo, allocInfo);
		}
		
		static std::pair<vk::Buffer, vma::Allocation> createBuffer(const vk::BufferCreateInfo& createInfo,
				const vma::AllocationCreateInfo& allocInfo)
		{
			return get().iCreateBuffer(createInfo, allocInfo);
		}
		
		static void destroyImage(vk::Image& image, vma::Allocation& allocation)
		{
			get().destroyImage(image, allocation);
		}
		static void destroyBuffer(vk::Buffer& buffer, vma::Allocation& allocation)
		{
			get().destroyBuffer(buffer, allocation);
		}

	private:
		MemoryAllocator() {} 
		~MemoryAllocator()
		{
			m_allocator.destroy();
		}

		void iInit(const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
				const vk::Instance& instance);
		std::pair<vk::Image, vma::Allocation> iCreateImage(const vk::ImageCreateInfo& createInfo,
				const vma::AllocationCreateInfo& allocInfo);
		std::pair<vk::Buffer, vma::Allocation> iCreateBuffer(const vk::BufferCreateInfo& createInfo,
				const vma::AllocationCreateInfo& allocInfo);
		void iDestroyImage(vk::Image& image, vma::Allocation& allocation);
		void iDestroyBuffer(vk::Buffer& buffer, vma::Allocation& allocation);

		vma::Allocator m_allocator;
	};

} /* dg */ 
