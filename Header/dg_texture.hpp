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

			Texture(Device& device, const std::string& filepath);
			~Texture()
			{
				gAllocator.destroyImage(m_image, m_allocation);
				m_device.device.destroySampler(sampler);
				m_device.device.destroyImageView(imageView);
			}

			vk::ImageView imageView;
			vk::Sampler sampler; 
			vk::ImageLayout imageLayout;

		private:
			void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
			void copyBufferToImage(Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height) const;

			Device& m_device;
			int m_width, m_height, m_mipLevels;
			vk::Image m_image;
			vma::Allocation m_allocation;
			vk::Format m_format;
	};
}
