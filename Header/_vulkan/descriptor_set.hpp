#pragma once
#include <vulkan/vulkan.hpp>

#include "_vulkan/vulkan_tool_box.hpp"

#include <vector>

namespace dg
{
  class DescriptorSet
  {
    public:

      static void allocate(std::vector<DescriptorSet>& descriptorSets,
          vk::DescriptorPool& pool, VulkanToolBox& toolBox);
      static void fetchWrites(std::vector<DescriptorSet>& descriptorSets,
          std::vector<vk::WriteDescriptorSet>& writes);
      static void fetchLayouts(std::vector<DescriptorSet>& descriptorSets,
          std::vector<vk::DescriptorSetLayout>& layouts);
      static void update(std::vector<DescriptorSet>& descriptorSets,
          VulkanToolBox& toolBox);

      DescriptorSet(VulkanToolBox& toolBox);

      // Layout creation
      void addBinding(vk::DescriptorType descriptorType,
          vk::ShaderStageFlags shaderStages = vk::ShaderStageFlagBits::eAll,
          uint32_t arraySize = 1, vk::DescriptorBindingFlags flags = {});
      void createLayout();

      // Update
      void write(uint32_t binding,
          const vk::ArrayProxyNoTemporaries<const vk::DescriptorImageInfo>& imageInfo,
          const vk::ArrayProxyNoTemporaries<const vk::DescriptorBufferInfo>& bufferInfo,
          uint32_t firstElement = 0);

    private:

      VulkanToolBox& m_toolBox;
      vk::DescriptorSetLayout m_layout;
      vk::DescriptorSet m_descriptorSet;
      std::vector<vk::WriteDescriptorSet> m_writes;
      std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
      std::vector<vk::DescriptorBindingFlags> m_bindingFlags;

  };
}
