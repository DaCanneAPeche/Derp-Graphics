#include "_vulkan/descriptor_set.hpp"

namespace dg
{

  void DescriptorSet::allocate(std::vector<DescriptorSet>& descriptorSets,
      vk::DescriptorPool& pool, VulkanToolBox& toolBox)
  {
    std::vector<vk::DescriptorSetLayout> layouts;
    fetchLayouts(descriptorSets, layouts);

    vk::DescriptorSetAllocateInfo allocInfo(pool, layouts);
    std::vector<vk::DescriptorSet> sets =
      toolBox.device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0 ; i < descriptorSets.size() ; i++)
      descriptorSets[i].m_descriptorSet = sets[i];
  }

  void DescriptorSet::fetchWrites(std::vector<DescriptorSet>& descriptorSets,
      std::vector<vk::WriteDescriptorSet>& writes)
  {
    assert(writes.size() == 0 && "Please give an empty vector");
    for (const auto& descriptorSet : descriptorSets)
      writes.insert(writes.end(), descriptorSet.m_writes.begin(),
          descriptorSet.m_writes.end());
  }

  void DescriptorSet::fetchLayouts(std::vector<DescriptorSet>& descriptorSets,
      std::vector<vk::DescriptorSetLayout>& layouts)
  {
    assert(layouts.size() == 0 && "Please give an empty vector");
    layouts.resize(descriptorSets.size());
    for (size_t i = 0 ; i < layouts.size() ; i++)
      layouts[i] = descriptorSets[i].m_layout;
  }

  // TODO : add support to copy sets
  void DescriptorSet::update(std::vector<DescriptorSet>& descriptorSets,
      VulkanToolBox& toolBox)
  {
    std::vector<vk::WriteDescriptorSet> writes;
    fetchWrites(descriptorSets, writes);
    toolBox.device.updateDescriptorSets(writes, {});
  }

  DescriptorSet::DescriptorSet(VulkanToolBox& toolBox) : m_toolBox(toolBox) {}

  // Layout creation
  void DescriptorSet::addBinding(vk::DescriptorType descriptorType,
      vk::ShaderStageFlags shaderStages, uint32_t arraySize,
      vk::DescriptorBindingFlags flags)
  {
    m_bindings.push_back(
        vk::DescriptorSetLayoutBinding(
          m_bindings.size(), descriptorType, arraySize, shaderStages
          )
        );

    m_bindingFlags.push_back(flags);
  }

  void DescriptorSet::createLayout()
  {
    vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlags(m_bindingFlags);
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, m_bindings, &bindingFlags);
    m_layout = m_toolBox.device.createDescriptorSetLayout(layoutInfo);
  }

  // Update
  void DescriptorSet::write(
      uint32_t binding,
      const vk::ArrayProxyNoTemporaries<const vk::DescriptorImageInfo>& imageInfo,
      const vk::ArrayProxyNoTemporaries<const vk::DescriptorBufferInfo>& bufferInfo,
      uint32_t firstElement)
  {
    m_writes.push_back(
        vk::WriteDescriptorSet(
          m_descriptorSet, binding, firstElement,
          m_bindings[binding].descriptorType, imageInfo, bufferInfo
          )
        );
  }
}
