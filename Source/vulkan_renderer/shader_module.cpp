#include "vulkan_renderer/shader_module.hpp"
#include "utils/file.hpp"
#include "spirv_cross/spirv_cross.hpp"

#include <iostream>

namespace dg
{

  void ShaderModule::create(VulkanToolBox& toolBox, vk::ShaderModule& shaderModule)
  {
    vk::ShaderModuleCreateInfo createInfo({}, spirvCode);
    shaderModule = toolBox.device.createShaderModule(createInfo);
  }

  void ShaderModule::reflect()
  {
    spirv_cross::Compiler compiler(spirvCode);
    spirv_cross::ShaderResources ressources = compiler.get_shader_resources();

    for (const auto& ubo : ressources.uniform_buffers)
    {
      std::cout << ubo.name << std::endl;
      std::cout << compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet) << std::endl;
    }
    for (const auto& img : ressources.separate_images)
    {
      std::cout << img.name << std::endl;
      std::cout << compiler.get_decoration(img.id, spv::DecorationDescriptorSet) << std::endl;
    }
  }

  ShaderModule ShaderModule::fromSpirv(const std::string &filePath)
  {
    ShaderModule shaderModule;
    shaderModule.spirvCode = file::read<uint32_t>(filePath);
    shaderModule.reflect();
    return shaderModule;
  }

}
