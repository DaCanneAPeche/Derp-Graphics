#include "_vulkan/descriptor_set_layout.hpp"

namespace dg
{
  DescriptorSetLayout& DescriptorSetLayout::addBinding(vk::DescriptorType descriptorType,
      vk::ShaderStageFlags shaderStages, uint32_t arraySize,
      vk::DescriptorBindingFlags flags)
  {
    m_bindings.push_back(
        vk::DescriptorSetLayoutBinding(
          m_bindings.size(), descriptorType, arraySize, shaderStages
          )
        );

    m_bindingFlags.push_back(flags);

    return *this;
  }

  void DescriptorSetLayout::create()
  {
    vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlags(m_bindingFlags);
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, m_bindings, &bindingFlags);
    layout = m_toolBox.device.createDescriptorSetLayout(layoutInfo);
  }

  vk::DescriptorType& DescriptorSetLayout::getBindingType(size_t index)
  {
    return m_bindings[index].descriptorType;
  }

  void DescriptorSetLayout::clean()
  {
    m_toolBox.device.destroy(layout);
  }
}
