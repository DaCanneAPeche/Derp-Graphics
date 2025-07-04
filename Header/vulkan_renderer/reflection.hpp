#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>

#include "vulkan/vulkan.hpp"
#include "slang.h"
#include "rfl.hpp"
#include "plog/Log.h"

namespace dg
{

  struct ShaderVariableDescription
  {
    std::string name;
    uint32_t size;
    uint32_t relativeOffset;
  };

  using UniformBuffer = std::vector<ShaderVariableDescription>;

  struct DescriptorSlot
  {
    std::string name;
    uint32_t set;
    uint32_t binding;
    vk::DescriptorType type;
    uint32_t arrayCount;
    size_t uboIndex;
  };

  struct ShaderDescription
  {
    std::vector<DescriptorSlot> descriptorSlots;
    std::vector<UniformBuffer> uniformBuffers;

    void print();
    
    void addDescriptorSlot(const std::string& name, uint32_t set, uint32_t binding,
        vk::DescriptorType type, uint32_t arrayCount, size_t uboIndex = 0);

    void addDescriptorSlotFromSlang(const std::string& name, uint32_t set,
        uint32_t binding, slang::BindingType type, uint32_t arrayCount, size_t uboIndex = 0);

    size_t addUBO(const UniformBuffer& ubo)
    {
      uniformBuffers.push_back(ubo);
      return uniformBuffers.size() - 1;
    }
  };

}
