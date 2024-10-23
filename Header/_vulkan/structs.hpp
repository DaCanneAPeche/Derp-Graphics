#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices
{
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  bool isComplete() const
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

