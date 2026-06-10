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

    void pushConstant(const auto& data, size_t rangeIndex = 0)
    {
      renderer.pushConstant(commandBuffer, data, rangeIndex);
    }

    template <CastableTo<uint32_t> PipelineIdType>
    void bindPipeline(PipelineIdType pipelineId)
    {
      renderer.bindPipeline(commandBuffer, pipelineId);
    }

    void submit()
    {
      renderer.endFrame(*this);
    }

  }; // Frame

} // dg
