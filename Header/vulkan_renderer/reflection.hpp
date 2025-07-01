#pragma once

#include <iostream>
#include <unordered_map>
#include "vulkan/vulkan.hpp"
#include "slang.h"
#include "rfl.hpp"

namespace dg
{

  struct DescriptorSlot
  {
    std::string name;
    uint32_t set;
    uint32_t binding;
    vk::DescriptorType type;
    uint32_t arrayCount;
  };

  struct ShaderDescription
  {
    std::vector<DescriptorSlot> descriptorSlots;

    void print()
    {
      std::cout << "Descriptor slots : " << std::endl;
      for (const auto& slot : descriptorSlots)
      {
        std::cout << slot.name << '('
          << "Set = " << slot.set << " ; Binding = " << slot.binding
          << " ; Type = " << rfl::enum_to_string(slot.type);

        if (slot.arrayCount) std::cout << " ; Array size = " << slot.arrayCount;

        std::cout << ')' <<std::endl;
      }
    }
    
    void addDescriptorSlot(const std::string& name, uint32_t set, uint32_t binding,
        vk::DescriptorType type, uint32_t arrayCount = 0)
    {
      descriptorSlots.push_back({ name, set, binding, type, arrayCount });
    }

    void addDescriptorSlotFromSlang(const std::string& name, uint32_t set,
        uint32_t binding, slang::BindingType type, uint32_t arrayCount = 0)
    {
      std::unordered_map<slang::BindingType, vk::DescriptorType> conversionMap = {
        {slang::BindingType::Sampler, vk::DescriptorType::eSampler},
        {slang::BindingType::Texture, vk::DescriptorType::eSampledImage},
        {slang::BindingType::ConstantBuffer, vk::DescriptorType::eUniformBuffer}
      };

      if (!conversionMap.contains(type))
        throw std::runtime_error("Unsupported descritor type during reflection");

      addDescriptorSlot(name, set, binding, conversionMap[type], arrayCount);
    }
  };

}
