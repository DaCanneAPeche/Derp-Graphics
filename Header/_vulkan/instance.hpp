#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "_vulkan/debug_messenger.hpp"

namespace dg
{
  vk::Instance createInstance(std::vector<const char*> requestedExtensions,
      std::vector<const char*> validationLayers, vk::ApplicationInfo& appInfo);
}
