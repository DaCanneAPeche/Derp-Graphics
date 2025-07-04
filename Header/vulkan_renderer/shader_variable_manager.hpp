#pragma once

#include "vulkan_renderer/renderer.hpp"

namespace dg
{
  class ShaderVariable;

  struct BufferWriteDescription
  {
    vk::DeviceSize offset, size;
    void* data;
  };

  /* 
   * A class built over an uniform buffer object to manage the variables given
   * to the shader. Should be built around shader reflection.
   * */
  class ShaderVariableManager
  {
    public:

      ShaderVariableManager(VulkanToolBox& toolBox,
          Renderer& renderer);
      
      void init(const std::string& descriptorName);
      void updateVariable(uint32_t offset, uint32_t size, void* data);
      void processWrites();
      std::unique_ptr<Buffer> uniformBuffer;

      ShaderVariable& operator[](const std::string& name)
      {
        return m_variables.at(name);
      }

    private:
      std::unordered_map<std::string, ShaderVariable> m_variables;
      std::vector<BufferWriteDescription> m_bufferWrites;
      Renderer& m_renderer;
      VulkanToolBox& m_toolBox;

      uint32_t createVariables(const std::string& name);
  };

  /*
   * A class reperesenting a UBO variable. 
   * */
  class ShaderVariable
  {
    public:
      uint32_t size = 0;
      uint32_t offset = 0;
      std::any value = 0;

      ShaderVariable(uint32_t _size, uint32_t _offset, ShaderVariableManager* pManager)
      : size(_size), offset(_offset), m_pManager(pManager) {}

      // Should be replaced by a operator= of some form ?
      template <class T>
      void setValue(T& newValue)
      {
        assert(m_pManager != nullptr && "m_pManager was not assigned");

        value = newValue;
        m_pManager->updateVariable(offset, size, &newValue);
      }

    private:
      ShaderVariableManager* m_pManager;
  };


}
