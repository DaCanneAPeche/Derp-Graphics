#pragma once

#include <vulkan/vulkan.hpp>

#include "_vulkan/vulkan_tool_box.hpp"

namespace dg
{

  class Buffer
  {

    public:

      Buffer(
          VulkanToolBox& vulkanToolBox, vk::DeviceSize instanceSize,
          uint32_t instanceCount, vk::BufferUsageFlags bufferUsageFlags,
          vma::AllocationCreateFlags allocFlag,
          vma::MemoryUsage memoryUsage = vma::MemoryUsage::eAuto,
          vk::DeviceSize minOffsetAlignement = 1,
          vk::SharingMode sharingMode = vk::SharingMode::eExclusive
          );

      ~Buffer();
      
      void copyToBuffer(Buffer& otherBuffer, vk::DeviceSize size);
      void copyToImage(vk::Image& image, uint32_t width, uint32_t height);

      void write(void* data, vk::DeviceSize offset = 0, vk::DeviceSize = vk::WholeSize);

      vk::Buffer buffer;
      vma::Allocation allocation;

    protected:

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

  template <class T>
  class SpecialisedBuffer : public Buffer
  {
    public:

      SpecialisedBuffer(
          VulkanToolBox& vulkanToolBox, 
          uint32_t instanceCount, vk::BufferUsageFlags bufferUsageFlags,
          vma::AllocationCreateFlags allocFlag,
          vma::MemoryUsage memoryUsage = vma::MemoryUsage::eAuto,
          vk::DeviceSize minOffsetAlignement = 1,
          vk::SharingMode sharingMode = vk::SharingMode::eExclusive
          ) : Buffer(vulkanToolBox, sizeof(T), instanceCount, bufferUsageFlags,
            allocFlag, memoryUsage, minOffsetAlignement, sharingMode) {}

      void write(T& data)
      {
        assert(buffer && "Buffer can't be written to before creation");
        m_toolBox.allocator.copyMemoryToAllocation(&data, allocation, 0, m_instanceSize);
      }

      void write(const std::vector<T>& data)
      {
        assert(buffer && "Buffer can't be written to before creation");
        m_toolBox.allocator.copyMemoryToAllocation(data.data(), allocation, 0,
            m_instanceSize * data.size());
      }
      
      vk::DescriptorBufferInfo descriptorInfo() const
      {
        return vk::DescriptorBufferInfo(buffer, 0, m_instanceSize);
      }
      
      void writeToIndex(T& data, int index)
      {
        m_toolBox.allocator.copyMemoryToAllocation(&data, allocation,
            m_instanceSize * index, m_instanceSize);
      }
      
      vk::DescriptorBufferInfo descriptorInfoForIndex(int index) const
      {
        return descriptorInfo(m_alignementSize, m_alignementSize * index);
      }

    private:

  };

}/* dg */

