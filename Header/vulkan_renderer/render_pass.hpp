#pragma once

#include "vulkan/vulkan.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include <vector>

namespace dg
{

  struct AttachmentReference
  {
    uint32_t index;

    vk::AttachmentReference get(vk::ImageLayout layout)
    {
      return vk::AttachmentReference(index, layout);
    }
  };

  class RenderPass
  {

    public:
      RenderPass(VulkanToolBox& vulkanToolBox) : m_toolBox(vulkanToolBox) {}
      ~RenderPass()
      {
        m_toolBox.device.destroy(renderPass);
      }

      vk::RenderPass renderPass;

      AttachmentReference addAttachment(vk::Format format, vk::SampleCountFlagBits samples,
          vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
          vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp,
          vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);

      RenderPass& addSubpass(const vk::SubpassDescription& subpass)
      {
        m_subpasses.push_back(subpass);
        return *this;
      }

      void create();

    private:

      VulkanToolBox& m_toolBox;
      std::vector<vk::AttachmentDescription> m_attachments = {};
      std::vector<vk::SubpassDescription> m_subpasses = {};

  };
}
