#pragma once

#include "_vulkan/descriptor_set_layout.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

#include <vector>

namespace dg
{

  using DescriptorSetIndex = size_t;

  class DescriptorSetManager
  {
    public:

      DescriptorSetManager(dg::VulkanToolBox& toolBox) : m_toolBox(toolBox) {}
      void clean()
      {
        layouts.clear();
        for (auto& layout : m_layouts) layout.clean();
      }

      DescriptorSetLayout& addLayout();
      DescriptorSetIndex addDescriptor(DescriptorSetLayoutIndex layoutIndex);

      void writeToDescriptor(uint32_t descriptorSetIndex, uint32_t binding,
          const vk::ArrayProxyNoTemporaries<const vk::DescriptorImageInfo>& imageInfo = {},
          const vk::ArrayProxyNoTemporaries<const vk::DescriptorBufferInfo>& bufferInfo = {},
          uint32_t firstElement = 0);

      void update();
      void allocate(vk::DescriptorPool& pool);

      std::vector<vk::DescriptorSet> descriptorSets;
      std::vector<vk::DescriptorSetLayout> layouts;

    private:

      std::vector<vk::WriteDescriptorSet> m_writes;
      std::vector<DescriptorSetLayout> m_layouts;
      std::vector<size_t> m_layoutIndexPerDescriptorSet;
      dg::VulkanToolBox& m_toolBox;
  };
}
