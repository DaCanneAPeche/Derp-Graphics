#include "dg_texture.hpp"

namespace dg {

	Texture::Texture(Device& device, const std::string& filepath) : m_device(device)
	{
		int channels;
		int desiredChannels = STBI_rgb_alpha;

		stbi_uc* pixels = stbi_load(filepath.c_str(), &m_width, &m_height, &channels, desiredChannels);
		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;

		if (!pixels)
			throw std::runtime_error("Error loading image");

		uint32_t pixelsCount = m_width * m_height;

		Buffer stagingBuffer(m_device, desiredChannels, pixelsCount,
				vk::BufferUsageFlagBits::eTransferSrc, vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
		stagingBuffer.write(pixels, desiredChannels * pixelsCount);
		stbi_image_free(pixels);

		m_format = vk::Format::eR8G8B8A8Srgb;

		vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, m_format, vk::Extent3D(m_width, m_height, 1),
				1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive);	
		
		vma::AllocationCreateInfo allocInfo({}, vma::MemoryUsage::eAuto);

		auto handle = gAllocator.createImage(imageInfo, allocInfo);
		m_image = handle.first;
		m_allocation = handle.second;

		transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		copyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
		transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void Texture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		vk::CommandBuffer commandBuffer = m_device.beginSingleTimeCommands();

		vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageMemoryBarrier barrier({}, {}, // determined later
				oldLayout, newLayout, vk::QueueFamilyIgnored, vk::QueueFamilyIgnored,
				m_image, subresourceRange);

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
		
		m_device.endSingleTimeCommands(commandBuffer);
	}
	
	void Texture::copyBufferToImage(Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height) const
	{
		vk::CommandBuffer commandBuffer = m_device.beginSingleTimeCommands();

		vk::ImageSubresourceLayers subresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
		vk::BufferImageCopy region(0, 0, 0, subresourceLayers, {0, 0, 0}, {width, height, 1});
		commandBuffer.copyBufferToImage(buffer.buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

		m_device.endSingleTimeCommands(commandBuffer);
	}
	

} // dg 
