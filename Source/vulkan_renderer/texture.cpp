#include "vulkan_renderer/texture.hpp"

namespace dg {

	Texture::Texture(VulkanToolBox& toolBox, const std::string& filepath,
      const vk::Sampler& _sampler) :
		m_toolBox(toolBox), c_subresourceRange(vk::ImageAspectFlagBits::eColor,
        0, 1, 0, 1), sampler(_sampler)
	{
		createImage(filepath);
		createImageView();
	}

	void Texture::createImage(const std::string& filepath)
	{
		int channels;
		int desiredChannels = STBI_rgb;

		stbi_uc* pixels = stbi_load(filepath.c_str(), &width, &height, &channels, desiredChannels);
		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		if (!pixels)
			throw std::runtime_error("Error loading image");

		uint32_t pixelsCount = width * height;

		Buffer stagingBuffer(
        m_toolBox, desiredChannels, pixelsCount,
				vk::BufferUsageFlagBits::eTransferSrc,
        vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
		stagingBuffer.write(pixels, desiredChannels * pixelsCount);
		stbi_image_free(pixels);

		vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, c_format, vk::Extent3D(width, height, 1),
				1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive);	
		
		vma::AllocationCreateInfo allocInfo({}, vma::MemoryUsage::eAuto);

		auto handle = gAllocator.createImage(imageInfo, allocInfo);
		m_image = handle.first;
		m_allocation = handle.second;

		transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    stagingBuffer.copyToImage(m_image, width, height);
		transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void Texture::createImageView()
	{
		vk::ImageViewCreateInfo imageViewInfo({}, m_image, vk::ImageViewType::e2D, c_format,
				vk::ComponentSwizzle::eIdentity, c_subresourceRange);

		imageView = m_toolBox.device.createImageView(imageViewInfo);
	}

	void Texture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		vk::CommandBuffer commandBuffer = m_toolBox.beginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier({}, {}, // determined later
				oldLayout, newLayout, vk::QueueFamilyIgnored, vk::QueueFamilyIgnored,
				m_image, c_subresourceRange);

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else throw std::runtime_error("Unsupported layout transition");

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
		
		m_toolBox.endSingleTimeCommands(commandBuffer);
	}
	
} // dg 
