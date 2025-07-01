#pragma once

#include <iostream>
#include <unordered_map>
#include <any>
#include <map>
#include <memory>

#include "vulkan/vulkan.hpp"
#include "slang.h"
#include "rfl.hpp"
#include "_vulkan/buffer.hpp"
#include "plog/Log.h"
#include "_vulkan/vulkan_tool_box.hpp"

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

  // UBO management

  class ShaderVariableManager;

  struct BufferWriteDescription
  {
    vk::DeviceSize offset, size;
    void* data;
  };

  /*
   * A class reperesenting a UBO variable. 
   * */
  class ShaderVariable
  {
    public:
      uint32_t size = 0;
      uint32_t index = 0;
      std::any value = 0;

      // Should be replaced by a operator= of some form ?
      void setValue(const std::any& newValue);

    private:
      ShaderVariableManager* m_pManager = nullptr;
  };

  /* 
   * A class built over an uniform buffer object to manage the variables given
   * to the shader. Should be built around shader reflection.
   * */
  class ShaderVariableManager
  {
    public:

      ShaderVariableManager(VulkanToolBox& vulkanToolBox);

      void updateVariable(size_t index, uint32_t size, void* data);
      void processWrites();

      ShaderVariable& operator[](const std::string& name)
      {
        return m_variables[name];
      }

    private:
      std::unique_ptr<Buffer> m_uniformBuffer;
      std::map<std::string, ShaderVariable> m_variables;
      std::vector<BufferWriteDescription> m_bufferWrites;
      VulkanToolBox& m_toolBox;
  };

}
