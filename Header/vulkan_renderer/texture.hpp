#pragma once
#include "stb/stb_image.h"
#include "vulkan/vulkan.hpp"
#include "_vulkan/buffer.hpp"
#include <string>
#include "_vulkan/vulkan_tool_box.hpp"

namespace dg
{
	class Texture {
		public:

			Texture(VulkanToolBox& toolBox, const std::string& filepath,
          vk::ImageUsageFlags imageUsage);
			~Texture()
			{
				m_toolBox.allocator.destroyImage(m_image, m_allocation);
				m_toolBox.device.destroyImageView(imageView);
			}

      float getRatio() { return static_cast<float>(width) / static_cast<float>(height); }

			vk::ImageView imageView;
			vk::ImageLayout imageLayout;
      int width, height;

		private:
			void createImage(const std::string& filepath);
			void createImageView();
			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

			VulkanToolBox& m_toolBox;
			int m_mipLevels;
			vk::Image m_image;
			vma::Allocation m_allocation;
			const vk::Format c_format = vk::Format::eR8G8B8Srgb;
			const vk::ImageSubresourceRange c_subresourceRange;
      vk::ImageUsageFlags m_imageUsage;
	};
}
