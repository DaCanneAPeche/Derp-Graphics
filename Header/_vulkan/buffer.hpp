#pragma once

#include <vulkan/vulkan.hpp>

#include "_vulkan/vulkan_tool_box.hpp"

namespace dg
{

	class Buffer
	{
	public:

		Buffer(
        VulkanToolBox& vulkanToolBox,
				vk::DeviceSize instanceSize,
				uint32_t instanceCount,
				vk::BufferUsageFlags bufferUsageFlags,
				vma::AllocationCreateFlags allocFlag,
				vma::MemoryUsage memoryUsage = vma::MemoryUsage::eAuto,
				vk::DeviceSize minOffsetAlignement = 1,
				vk::SharingMode sharingMode = vk::SharingMode::eExclusive
				);
		~Buffer ();

		void write(void* data, vk::DeviceSize size, vk::DeviceSize offset = 0);
		[[nodiscard]] vk::DescriptorBufferInfo descriptorInfo(vk::DeviceSize size = vk::WholeSize, vk::DeviceSize offset = 0) const;
		void writeToIndex(void* data, int index);
		[[nodiscard]] vk::DescriptorBufferInfo descriptorInfoForIndex(int index) const;

    void copyToBuffer(Buffer& otherBuffer, vk::DeviceSize size);
    void copyToImage(vk::Image& image, uint32_t width, uint32_t height);

		vk::Buffer buffer;
		vma::Allocation allocation;

	private:
		vk::BufferUsageFlags m_bufferUsageFlags;
		vma::AllocationCreateFlags m_allocFlag;
		vk::SharingMode m_sharingMode;
		vma::MemoryUsage m_memoryUsage;
		vk::DeviceSize m_bufferSize;
		vk::DeviceSize m_instanceSize;
		vk::DeviceSize m_alignementSize;
    VulkanToolBox& m_toolBox;

		void createBuffer();
		[[nodiscard]] vk::DeviceSize getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignement) const;
	};

}/* dg */
