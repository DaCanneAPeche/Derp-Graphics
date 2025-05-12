#pragma once

#include <vector>
#include <string>
#include "vulkan/vulkan.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

namespace dg
{
  struct ShaderModule
  {
    // vk::ShaderStageFlagBits shaderStage = {};
    std::vector<uint32_t> spirvCode = {};

    void create(VulkanToolBox& toolBox, vk::ShaderModule& shaderModule);

    static ShaderModule fromSpirv(const std::string& filePath);
    static ShaderModule fromGlsl(const std::string& filePath);
    static ShaderModule fromSlang(const std::string& filePath, vk::ShaderStageFlagBits shaderStage);
    static std::array<ShaderModule, 2> fromSlang(const std::string& filePath);

  };
}
