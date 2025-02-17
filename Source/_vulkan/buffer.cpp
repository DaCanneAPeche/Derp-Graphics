#include "_vulkan/buffer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include <iostream>
#include <plog/Log.h>

namespace dg
{
	Buffer::Buffer(
      VulkanToolBox& vulkanToolBox,
      vk::DeviceSize instanceSize,
			uint32_t instanceCount,
			vk::BufferUsageFlags bufferUsageFlags,
			vma::AllocationCreateFlags allocFlag,
			vma::MemoryUsage memoryUsage,
			vk::DeviceSize minOffsetAlignement,
			vk::SharingMode sharingMode
			) : m_bufferUsageFlags(bufferUsageFlags), m_sharingMode(sharingMode),
					m_memoryUsage(memoryUsage), m_instanceSize(instanceSize),
          m_allocFlag(allocFlag), m_toolBox(vulkanToolBox)
	{
		m_alignementSize = getAlignment(m_instanceSize, minOffsetAlignement);
		m_bufferSize = m_alignementSize * instanceCount;
		createBuffer();
	}

	Buffer::~Buffer()
	{
		m_toolBox.allocator.destroyBuffer(buffer, allocation);
	}
  
	void Buffer::createBuffer()
	{
		vk::BufferCreateInfo bufferInfo({}, m_bufferSize, m_bufferUsageFlags, m_sharingMode);
		vma::AllocationCreateInfo allocInfo(m_allocFlag, m_memoryUsage);
		auto handle = m_toolBox.allocator.createBuffer(bufferInfo, allocInfo);
		buffer = handle.first;
		allocation = handle.second;
	}

  void Buffer::copyToBuffer(Buffer& otherBuffer, vk::DeviceSize size)
  {
		vk::CommandBuffer commandBuffer = m_toolBox.beginSingleTimeCommands();

    vk::BufferCopy copyRegion(0, 0, size);
    commandBuffer.copyBuffer(buffer, otherBuffer.buffer, copyRegion);

		m_toolBox.endSingleTimeCommands(commandBuffer);
  }
  
  void Buffer::copyToImage(vk::Image& image, uint32_t width, uint32_t height)
  {
		vk::CommandBuffer commandBuffer = m_toolBox.beginSingleTimeCommands();

		vk::ImageSubresourceLayers subresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
		vk::BufferImageCopy region(0, 0, 0, subresourceLayers, {0, 0, 0}, {width, height, 1});
		commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

		m_toolBox.endSingleTimeCommands(commandBuffer);
  }
  
  void Buffer::write(void* pData, vk::DeviceSize offset, vk::DeviceSize size)
  {
		assert(buffer && "Buffer can't be written to before creation");
    m_toolBox.allocator.copyMemoryToAllocation(pData, allocation, offset, size);
  }
  
	vk::DeviceSize Buffer::getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment) const
	{
		if (minOffsetAlignment > 0)
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		
		return instanceSize;
	}
  
}
