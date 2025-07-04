#include "vulkan_renderer/shader_variable_manager.hpp"

namespace dg
{
  ShaderVariableManager::ShaderVariableManager(VulkanToolBox& toolBox,
      Renderer& renderer)
    : m_renderer(renderer), m_toolBox(toolBox) {}
  
  void ShaderVariableManager::init(const std::string& descriptorName)
  {
    // NOTE : vk::DeviceSize = uint64_t, but the manager handles uint32_t, need to
    // change that to avoid possible (altought improbale) bugs
    vk::DeviceSize size = createVariables(descriptorName);

    uniformBuffer = std::make_unique<Buffer>(m_toolBox, size, 1, vk::BufferUsageFlagBits::eUniformBuffer,
        vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
  }

  void ShaderVariableManager::updateVariable(uint32_t offset, uint32_t size, void* data)
  {
    BufferWriteDescription writeDescription {offset, size, data};
    m_bufferWrites.push_back(writeDescription);
  }

  void ShaderVariableManager::processWrites()
  {
    for(const BufferWriteDescription& bufferWrite : m_bufferWrites)
    {
      uniformBuffer->write(bufferWrite.data, bufferWrite.offset, bufferWrite.size);
    }
    m_bufferWrites.clear();
  }

  uint32_t ShaderVariableManager::createVariables(const std::string& name)
  {
    size_t structIndex = m_renderer.descriptors[name].uboIndex;
    UniformBuffer& layout = m_renderer.shaderDescription.uniformBuffers[structIndex];

    uint32_t offset = 0;
    for (const ShaderVariableDescription& variable : layout)
    {
      ShaderVariable var(variable.size, offset + variable.relativeOffset, this);
      m_variables.insert({variable.name, std::move(var)});

      offset += variable.relativeOffset + variable.size;
    }
    
    return offset;
  }
}
