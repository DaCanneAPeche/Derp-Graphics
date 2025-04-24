#pragma once

#include "_vulkan/buffer.hpp"
#include "plog/Log.h"
#include "_vulkan/vulkan_tool_box.hpp"

#include <any>
#include <unordered_map>
#include <memory>

namespace dg
{
  class ShaderVariableManager;

  /*
   * A class reperesenting a UBO variable. 
   * */
  class ShaderVariable
  {
    public:
      uint32_t size = 0;
      std::any value = 0;

      ShaderVariable(ShaderVariableManager& manager) : m_manager(manager) {}

      // Should be replaced by a operator= of some form ?
      void setValue(const std::any& newValue)
      {
        value = newValue;
      }

    private:
      const ShaderVariableManager& m_manager;
  };

  /* 
   * A class built over an uniform buffer object to manage the variables given
   * to the shader. Should be built around shader reflection.
   * */
  class ShaderVariableManager
  {
    public:

      ShaderVariableManager(VulkanToolBox& vulkanToolBox) : m_toolBox(vulkanToolBox)
      {
        vk::DeviceSize size = 0;

        throw std::runtime_error("A ShaderVariableManager was created but the class isn't implemented yet");

        for (const auto& [key, variable] : m_variables)
        {
          if (variable.size == 0)
          {
            LOG_WARNING << "variable " << key << " has a null size";
          }

          size += variable.size;
        }

        m_uniformBuffer = std::make_unique<Buffer>(m_toolBox, size, 1, vk::BufferUsageFlagBits::eUniformBuffer,
            vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
      }

      ShaderVariable& operator[](const std::string& name)
      {
        return m_variables[name];
      }

    private:
      std::unique_ptr<Buffer> m_uniformBuffer;
      std::unordered_map<std::string, ShaderVariable> m_variables;
      VulkanToolBox& m_toolBox;

      void updateVariable(size_t index, uint32_t size, void* data)
      {
        uint32_t offset = 0;

        uint32_t _index = 0;
        for (const auto& [key, variable] : m_variables)
        {
          if (_index >= index) break;
          offset += variable.size;
        }

        m_uniformBuffer->write(data, offset, size);
      }

  };
}
