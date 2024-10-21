#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace dg
{
  vk::Instance createInstance(std::vector<const char*> requestedExtensions,
      std::vector<const char*> validationLayers, vk::ApplicationInfo& appInfo);
}
