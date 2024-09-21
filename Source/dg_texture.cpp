#include "dg_texture.hpp"

namespace dg {

	Texture::Texture(const std::string& path)
	{
		load(path);
	}

	void Texture::load(const std::string& path)
	{
		pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		size = width * height * 4;

		if (!pixels)
			throw std::runtime_error("Error loading image");
	}

	void Texture::createImage(Device& device)
	{
		Buffer stagingBuffer(device, size, 1, vk::BufferUsageFlagBits::eTransferSrc, 
				vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto);
		stagingBuffer.write(pixels, size);
		stbi_image_free(pixels);
		vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb, vk::Extent3D(width, height, 1),
				1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eSampled, vk::SharingMode::eExclusive);	
		
		vma::AllocationCreateInfo allocInfo({}, vma::MemoryUsage::eAuto);

		auto handle = gAllocator.createImage(imageInfo, allocInfo);
		image = handle.first;
		allocation = handle.second;
	}

} // dg 
