#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

#include "_vulkan/vulkan_tool_box.hpp"

namespace dg
{
  using DescriptorSetLayoutIndex = size_t;

  class DescriptorSetLayout
  {
    public:
      DescriptorSetLayout(dg::VulkanToolBox& toolBox, DescriptorSetLayoutIndex index)
        : m_toolBox(toolBox), m_index(index) {}

      DescriptorSetLayout& addBinding(vk::DescriptorType descriptorType,
          vk::ShaderStageFlags shaderStages = vk::ShaderStageFlagBits::eAll,
          uint32_t arraySize = 1, vk::DescriptorBindingFlags flags = {});
      DescriptorSetLayoutIndex create();
      vk::DescriptorType& getBindingType(size_t index);
      void clean();

      vk::DescriptorSetLayout layout;

    private:
      std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
      std::vector<vk::DescriptorBindingFlags> m_bindingFlags;
      dg::VulkanToolBox& m_toolBox;
      DescriptorSetLayoutIndex m_index;

  };
}
