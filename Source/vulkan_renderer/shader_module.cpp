#include "vulkan_renderer/shader_module.hpp"
#include "utils/file.hpp"

namespace dg
{

  void ShaderModule::create(VulkanToolBox& toolBox, vk::ShaderModule& shaderModule)
  {
    vk::ShaderModuleCreateInfo createInfo({}, spirvCode);
    shaderModule = toolBox.device.createShaderModule(createInfo);
  }

  ShaderModule ShaderModule::fromSpirv(const std::string &filePath)
  {
    ShaderModule shaderModule;
    shaderModule.spirvCode = file::read<uint32_t>(filePath);
    return shaderModule;
  }

}
