#pragma once

#include <vulkan/vulkan.hpp>

#include "dg_device.hpp"
#include "dg_globals.hpp"

namespace dg
{

	class Buffer
	{
	public:
		Buffer(
				Device& device,
				vk::DeviceSize instanceSize,
				uint32_t instanceCount,
				vk::BufferUsageFlagBits bufferUsageFlags,
				vma::AllocationCreateFlagBits allocFlag,
				vma::MemoryUsage memoryUsage = vma::MemoryUsage::eAuto,
				vk::DeviceSize minOffsetAlignement = 1,
				vk::SharingMode sharingMode = vk::SharingMode::eExclusive
				);
		~Buffer ();

		void write(void* data, vk::DeviceSize size, vk::DeviceSize offset = 0);
		vk::DescriptorBufferInfo descriptorInfo(vk::DeviceSize size = vk::WholeSize, vk::DeviceSize offset = 0);
		void writeToIndex(void* data, int index);
		vk::DescriptorBufferInfo descriptorInfoForIndex(int index);

		vk::Buffer buffer;
		vma::Allocation allocation;

	private:
		Device& m_device;
		vk::BufferUsageFlags m_bufferUsageFlags;
		vma::AllocationCreateFlags m_allocFlag;
		vk::SharingMode m_sharingMode;
		vma::MemoryUsage m_memoryUsage;
		vk::DeviceSize m_bufferSize;
		vk::DeviceSize m_instanceSize;
		vk::DeviceSize m_alignementSize;

		void createBuffer();
		vk::DeviceSize getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignement);
	};

}/* dg */
