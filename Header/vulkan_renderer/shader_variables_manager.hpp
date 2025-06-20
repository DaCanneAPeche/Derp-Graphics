#pragma once

#include "_vulkan/buffer.hpp"
#include "plog/Log.h"
#include "_vulkan/vulkan_tool_box.hpp"
#include "spirv_cross/spirv_cross.hpp"

#include <any>
#include <map>
#include <memory>

namespace dg
{
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

      ShaderVariableManager(VulkanToolBox& vulkanToolBox, spirv_cross::Resource& uboDescription);

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
