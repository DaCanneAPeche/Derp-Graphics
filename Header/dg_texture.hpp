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
			int width = 0;
			int height = 0;
			int channels = 0;
			vk::DeviceSize size;
			stbi_uc* pixels = nullptr;
			vk::Image image;
			vma::Allocation allocation;

			Texture(const std::string& path);
			~Texture()
			{
				gAllocator.destroyImage(image, allocation);
			}

			void createImage(Device& device);

		private:
			void load(const std::string& path);
	};
}
