#pragma once

#include "vulkan_renderer/renderer.hpp"

namespace dg
{

  struct Frame
  {
    Frame(Renderer& _renderer, vk::CommandBuffer& _commandBuffer,
        int _imageIndex)
      : renderer(_renderer), commandBuffer(_commandBuffer),
      imageIndex(_imageIndex) {} 
 
    vk::CommandBuffer& commandBuffer;
    Renderer& renderer;
    uint32_t imageIndex = 0;

    void pushConstant(const auto& data)
    {
      renderer.pushConstant(commandBuffer, data);
    }

    void bindPipeline(Pl pipelineId)
    {
      renderer.bindPipeline(commandBuffer, pipelineId);
    }

    void submit()
    {
      renderer.endFrame(*this);
    }

  }; // Frame

} // dg
