#include "vulkan_renderer/shader_variables_manager.hpp"

namespace dg
{
  void ShaderVariable::setValue(const std::any& newValue)
  {
    assert(m_pManager != nullptr && "m_pManager was not assigned");

    value = newValue;
    m_pManager->updateVariable(index, size, &value);
  }

  ShaderVariableManager::ShaderVariableManager(VulkanToolBox& vulkanToolBox) : m_toolBox(vulkanToolBox)
  {
    vk::DeviceSize size = 0;

    // implement shader reflection
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

  void ShaderVariableManager::updateVariable(size_t index, uint32_t size, void* data)
  {
    uint32_t offset = 0;

    uint32_t _index = 0;
    for (const auto& [key, variable] : m_variables)
    {
      if (_index >= index) break;
      offset += variable.size;
      _index++;
    }

    BufferWriteDescription writeDescription {offset, size, data};
    m_bufferWrites.push_back(writeDescription);
  }

  void ShaderVariableManager::processWrites()
  {
    for(const BufferWriteDescription& bufferWrite : m_bufferWrites)
      m_uniformBuffer->write(bufferWrite.data, bufferWrite.offset, bufferWrite.size);
    m_bufferWrites.clear();
  }
}

