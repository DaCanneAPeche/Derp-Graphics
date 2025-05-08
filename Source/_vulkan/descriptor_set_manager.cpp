#include "_vulkan/descriptor_set_manager.hpp"
#include <plog/Log.h>

namespace dg
{

  DescriptorSetLayout& DescriptorSetManager::addLayout()
  {
    m_layouts.emplace_back(m_toolBox, m_layouts.size());
    return m_layouts.back();
  }

  DescriptorSetIndex DescriptorSetManager::addDescriptor(DescriptorSetLayoutIndex layoutIndex)
  {
    m_layoutIndexPerDescriptorSet.push_back(layoutIndex);
    layouts.push_back(m_layouts[layoutIndex].layout);
    return layouts.size() - 1;
  }

  void DescriptorSetManager::writeToDescriptor(uint32_t descriptorSetIndex, uint32_t binding,
      const vk::ArrayProxyNoTemporaries<const vk::DescriptorImageInfo>& imageInfo,
      const vk::ArrayProxyNoTemporaries<const vk::DescriptorBufferInfo>& bufferInfo,
      uint32_t firstElement)
  {
    assert(descriptorSets.size() && "No descriptor was allocated");
    assert(descriptorSets.size() - 1 >= descriptorSetIndex && "That descriptor does not exist");

    m_writes.emplace_back(
          descriptorSets[descriptorSetIndex], binding, firstElement,
          m_layouts[m_layoutIndexPerDescriptorSet[descriptorSetIndex]].getBindingType(binding),
          imageInfo, bufferInfo
        );
  }

  void DescriptorSetManager::update()
  {
    m_toolBox.device.updateDescriptorSets(m_writes, {});
    m_writes.clear();
  }

  void DescriptorSetManager::allocate(vk::DescriptorPool& pool)
  {
    vk::DescriptorSetAllocateInfo allocInfo(pool, layouts);
    descriptorSets.resize(layouts.size());
    descriptorSets = m_toolBox.device.allocateDescriptorSets(allocInfo);
  }
}
