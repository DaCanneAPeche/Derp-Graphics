#pragma once
#include "stb/stb_image.h"
#include "vulkan/vulkan.hpp"
#include "dg_buffer.hpp"
#include <string>
#include "vk_mem_alloc.hpp"
#include "dg_device.hpp"
#include "dg_globals.hpp"

namespace dg
{
	class Texture {
		public:

			Texture(Device& device, const std::string& filepath, float maxAnistrophy,
          vk::Filter minifiedFilter = vk::Filter::eLinear,
          vk::Filter magnifiedFilter = vk::Filter::eLinear,
					vk::SamplerAddressMode adressingMode = vk::SamplerAddressMode::eClampToEdge);
			~Texture()
			{
				gAllocator.destroyImage(m_image, m_allocation);
				m_device.device.destroySampler(sampler);
				m_device.device.destroyImageView(imageView);
			}

			vk::ImageView imageView;
			vk::Sampler sampler; 
			vk::ImageLayout imageLayout;
      int width, height;

		private:
			void createImage(const std::string& filepath);
			void createImageView();
			void createSampler(float maxAnistrophy, vk::Filter minifiedFilter, vk::Filter magnifiedFilter,
					vk::SamplerAddressMode adressingMode);

			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
			void copyBufferToImage(Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height) const;

			Device& m_device;
			int m_mipLevels;
			vk::Image m_image;
			vma::Allocation m_allocation;
			const vk::Format c_format = vk::Format::eR8G8B8Srgb;
			const vk::ImageSubresourceRange c_subresourceRange;
	};
}
