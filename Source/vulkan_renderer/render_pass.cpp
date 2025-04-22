#include "vulkan_renderer/render_pass.hpp"

namespace dg
{

    AttachmentReference RenderPass::addAttachment(vk::Format format, vk::SampleCountFlagBits samples,
        vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
        vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp,
        vk::ImageLayout initialLayout, vk::ImageLayout finalLayout)
    {
      m_attachments.push_back(
          vk::AttachmentDescription(
            {}, format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp,
            initialLayout, finalLayout)
          );

      uint32_t index = static_cast<uint32_t>(m_attachments.size()) - 1;
      return {index};
    }

    void RenderPass::create()
    {
      vk::RenderPassCreateInfo createInfo({}, m_attachments, m_subpasses);
      renderPass = m_toolBox.device.createRenderPass(createInfo);
    }

}
