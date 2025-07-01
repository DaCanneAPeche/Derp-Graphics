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
        uint32_t binding, slang::TypeReflection::Kind kind, uint32_t arrayCount = 0)
    {
      using SK = slang::TypeReflection::Kind;
      vk::DescriptorType type;

      if (kind == SK::ConstantBuffer) type = vk::DescriptorType::eUniformBuffer;
      else if (kind == SK::SamplerState) type = vk::DescriptorType::eSampler;
      else if (kind == SK::Resource) type = vk::DescriptorType::eSampledImage;
      else if (kind == SK::Array) throw std::runtime_error("addDSFSlanName() does not take arrays");
      else throw std::runtime_error("Unsupported slang descriptor type while reflecting");

      addDescriptorSlot(name, set, binding, type, arrayCount);
    }
  };

}
