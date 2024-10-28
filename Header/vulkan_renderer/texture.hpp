#pragma once
#include "stb/stb_image.h"
#include "vulkan/vulkan.hpp"
#include "_vulkan/buffer.hpp"
#include <string>
#include "vk_mem_alloc.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/globals.hpp"

namespace dg
{
	class Texture {
		public:

			Texture(VulkanToolBox& toolBox, const std::string& filepath,
          const vk::Sampler& _sampler);
			~Texture()
			{
				gAllocator.destroyImage(m_image, m_allocation);
				m_toolBox.device.destroySampler(sampler);
				m_toolBox.device.destroyImageView(imageView);
			}

			vk::ImageView imageView;
			const vk::Sampler& sampler; 
			vk::ImageLayout imageLayout;
      int width, height;

		private:
			void createImage(const std::string& filepath);
			void createImageView();
			void createSampler(float maxAnistrophy, vk::Filter minifiedFilter, vk::Filter magnifiedFilter,
					vk::SamplerAddressMode adressingMode);

			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
			void copyBufferToImage(Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height) const;

			VulkanToolBox& m_toolBox;
			int m_mipLevels;
			vk::Image m_image;
			vma::Allocation m_allocation;
			const vk::Format c_format = vk::Format::eR8G8B8Srgb;
			const vk::ImageSubresourceRange c_subresourceRange;
	};
}
