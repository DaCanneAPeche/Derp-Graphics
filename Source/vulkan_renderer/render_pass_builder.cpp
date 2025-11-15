#include "vulkan_renderer/render_pass_builder.hpp"

namespace dg
{
  void RenderPassBuilder::BuildDefaultRenderpass(RenderPass& renderPass)
  {
    vk::AttachmentReference colorAttachmentReference = renderPass.addAttachment(
        renderPass.toolBox.getSwapChainSurfaceFormat().format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
        ).get(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference depthAttachmentReference = renderPass.addAttachment(
        renderPass.toolBox.findDepthFormat(),
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
        ).get(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    renderPass.addSubpass(vk::SubpassDescription(
          {}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference, {},
          &depthAttachmentReference, {}
          ));

    renderPass.addDependency(vk::SubpassDependency(
          vk::SubpassExternal, 0,
          vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
          vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
          {},
          vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
          {}
          ));

    renderPass.create();
  }
}
