#include "vulkan_renderer/reflection.hpp"

namespace dg
{
  void ShaderDescription::print()
  {
    std::cout << "Descriptor sets : " << descriptorSets.size() << std::endl;
    for (const auto& set : descriptorSets)
    {
      for (const auto& slot : set)
      {
        std::cout << slot.name << '('
          << "Set = " << slot.set << " ; Binding = " << slot.binding
          << " ; Type = " << rfl::enum_to_string(slot.type);

        if (slot.arrayCount > 1) std::cout << " ; Array size = " << slot.arrayCount;
        if (slot.type == vk::DescriptorType::eUniformBuffer)
        {
          std::cout << " ; UBO structure index = " << slot.uboIndex;
        }

        std::cout << ')' << std::endl;
      }
    }

    std::cout << std::endl;

    std::cout << "UBO structures : " << uniformBuffers.size() << std::endl;
    for (const auto& ubo : uniformBuffers)
    {
      std::cout << '-';
      for (const auto& field : ubo)
      {
        std::cout << ' ' << field.name << " (size = " << field.size
          << " ; offset = " << field.relativeOffset << ")" << std::endl;
      }
    }
  }

  void ShaderDescription::addDescriptorSlot(const std::string& name, uint32_t set,
      uint32_t binding, vk::DescriptorType type, uint32_t arrayCount, size_t uboIndex)
  {
    // Automaticly group descriptors by set
    DescriptorSlot emptySlot;
    if (set + 1 > descriptorSets.size()) // Set not in descritorSlots
      descriptorSets.resize(set + 1);

    if (binding + 1 > descriptorSets[set].size()) // Same but with binding
      descriptorSets[set].resize(binding + 1, emptySlot);

    descriptorSets[set][binding] = {name, set, binding, type, arrayCount, uboIndex};
  }

  void ShaderDescription::addDescriptorSlotFromSlang(const std::string& name, uint32_t set,
      uint32_t binding, slang::BindingType type, uint32_t arrayCount, size_t uboIndex)
  {
    std::unordered_map<slang::BindingType, vk::DescriptorType> conversionMap = {
      {slang::BindingType::Sampler, vk::DescriptorType::eSampler},
      {slang::BindingType::Texture, vk::DescriptorType::eSampledImage},
      {slang::BindingType::ConstantBuffer, vk::DescriptorType::eUniformBuffer}
    };

    if (!conversionMap.contains(type))
      throw std::runtime_error("Unsupported descritor type during reflection");

    addDescriptorSlot(name, set, binding, conversionMap[type], arrayCount, uboIndex);
  }
}

