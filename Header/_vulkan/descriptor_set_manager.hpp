#pragma once

#include "_vulkan/descriptor_set_layout.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

#include <vector>
#include <unordered_map>

namespace dg
{

  class DescriptorPool
  {
    public:
      void addToPool(vk::DescriptorType type, uint32_t amount); 
      void create(VulkanToolBox& toolBox, vk::DescriptorPoolCreateFlagBits flags = {});

      vk::DescriptorPool descriptorPool;
      uint32_t numberOfSets = 0;

    private:
      std::unordered_map<vk::DescriptorType, uint32_t> m_descriptorCount;
  };

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

  struct DescriptorWriter
  {
    uint32_t set, binding;
    DescriptorSetManager* pManager = nullptr;

    void writeToImage(const vk::ArrayProxyNoTemporaries<const vk::DescriptorImageInfo>& imageInfo,
        uint32_t firstElement = 0)
    {
      assert(pManager != nullptr && "pManager was not assigned");
      pManager->writeToDescriptor(set, binding, imageInfo, {}, firstElement);
    }

    void writeToBuffer(const vk::ArrayProxyNoTemporaries<const vk::DescriptorBufferInfo>& bufferInfo,
        uint32_t firstElement = 0)
    {
      assert(pManager != nullptr && "pManager was not assigned");
      pManager->writeToDescriptor(set, binding, {}, bufferInfo, firstElement);
    }
  };
}
