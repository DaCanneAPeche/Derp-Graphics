#pragma once

#include <vector>
#include <string>
#include "vulkan/vulkan.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "slang.h"
#include "slang-com-ptr.h"

namespace dg
{

  enum class ShaderSource
  {
    unknown = -1,
    spirv = 0,
    slang
  };

  struct ShaderModule
  {
    // vk::ShaderStageFlagBits shaderStage = {};

    ShaderSource source = ShaderSource::unknown;
    
    std::vector<uint32_t> spirvCode = {};
    Slang::ComPtr<slang::IBlob> slangSpirvCode;

    void create(VulkanToolBox& toolBox, vk::ShaderModule& shaderModule);
    void reflect();

    static ShaderModule fromSpirv(const std::string& filePath);
    /* static ShaderModule fromGlsl(const std::string& filePath); */
    /* static ShaderModule fromSlang(const std::string& filePath, vk::ShaderStageFlagBits shaderStage); */
    /* static std::array<ShaderModule, 2> fromSlang(const std::string& filePath); */

  };
}
