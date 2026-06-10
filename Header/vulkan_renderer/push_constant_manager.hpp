#pragma once

#include <stdint.h>
#include <plog/Log.h>
#include "vulkan/vulkan.hpp"

namespace dg
{
  struct PushConstantManager
  {
    size_t addRange(vk::ShaderStageFlags shaderStages, uint32_t offset, uint32_t size)
    {
      ranges.emplace_back(shaderStages, offset, size);
      return ranges.size() - 1;
    }

    // TODO : check for overlaps
    void fillRangesIfDoNotExistAtAll(uint32_t totalSize)
    {
      if (ranges.size() == 0)
      {
        LOG_INFO << "Filling up push constant ranges with size " << totalSize;
        addRange(vk::ShaderStageFlagBits::eAll, 0, totalSize);
      }
    }

    std::vector<vk::PushConstantRange> ranges;

  };
}
