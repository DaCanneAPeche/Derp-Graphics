#include "_vulkan/descriptor_set.hpp"

namespace dg
{

  void DescriptorSet::allocate(std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets,
      vk::DescriptorPool& pool, VulkanToolBox& toolBox)
  {
    std::vector<vk::DescriptorSetLayout> layouts;
    fetchLayouts(descriptorSets, layouts);

    vk::DescriptorSetAllocateInfo allocInfo(pool, layouts);
    std::vector<vk::DescriptorSet> sets =
      toolBox.device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0 ; i < descriptorSets.size() ; i++)
      descriptorSets[i]->m_descriptorSet = sets[i];
  }

  // NOTE : maybe use reference_wrapper to avoid copies ??
  void DescriptorSet::fetchWrites(std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets,
      std::vector<vk::WriteDescriptorSet>& writes)
  {
    assert(writes.size() == 0 && "Please give an empty vector");
    for (const auto& descriptorSet : descriptorSets)
      writes.insert(writes.end(), descriptorSet->m_writes.begin(),
          descriptorSet->m_writes.end());
  }

  // NOTE : maybe use reference_wrapper to avoid copies ??
  void DescriptorSet::fetchLayouts(std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets,
      std::vector<vk::DescriptorSetLayout>& layouts)
  {
    assert(layouts.size() == 0 && "Please give an empty vector");
    layouts.resize(descriptorSets.size());
    for (size_t i = 0 ; i < layouts.size() ; i++)
      layouts[i] = descriptorSets[i]->m_layout;
  }

  // TODO : add support to copy sets
  void DescriptorSet::update(std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets,
      VulkanToolBox& toolBox)
  {
    std::vector<vk::WriteDescriptorSet> writes;
    fetchWrites(descriptorSets, writes);
    toolBox.device.updateDescriptorSets(writes, {});
  }

  // NOTE : copying all descriptor sets EVERY frame kinda sucks
  // Maybe making that as a conversion operator would be nice
  void DescriptorSet::fetchRawSets(std::vector<std::unique_ptr<DescriptorSet>>& descriptorSets,
      std::vector<vk::DescriptorSet>& rawSets)
  {
    assert(rawSets.size() == 0 && "Please give an empty vector");
    rawSets.resize(descriptorSets.size());

    for (size_t i = 0 ; i < descriptorSets.size() ; i++)
      rawSets[i] = descriptorSets[i]->m_descriptorSet;
  }

  DescriptorSet::DescriptorSet(VulkanToolBox& toolBox,
      vk::DescriptorSet& rawDescriptorSet) : m_toolBox(toolBox),
      m_descriptorSet(rawDescriptorSet) {}

  // Layout creation
  DescriptorSet& DescriptorSet::addBinding(vk::DescriptorType descriptorType,
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
