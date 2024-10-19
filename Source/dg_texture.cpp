#include "dg_texture.hpp"

namespace dg {

	Texture::Texture(Device& device, const std::string& filepath, float maxAnistrophy, vk::Filter minifiedFilter,
			vk::Filter magnifiedFilter,
			vk::SamplerAddressMode adressingMode) :
		m_device(device), c_subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	{
		createImage(filepath);
		createImageView();
		createSampler(maxAnistrophy, minifiedFilter, magnifiedFilter, adressingMode);
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

		Buffer stagingBuffer(m_device, desiredChannels, pixelsCount,
				vk::BufferUsageFlagBits::eTransferSrc, vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
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
		copyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void Texture::createImageView()
	{
		vk::ImageViewCreateInfo imageViewInfo({}, m_image, vk::ImageViewType::e2D, c_format,
				vk::ComponentSwizzle::eIdentity, c_subresourceRange);

		imageView = m_device.device.createImageView(imageViewInfo);
	}

	void Texture::createSampler(float maxAnistrophy, vk::Filter minifiedFilter, vk::Filter magnifiedFilter,
			vk::SamplerAddressMode adressingMode)
	{
		vk::SamplerCreateInfo samplerInfo({}, magnifiedFilter, minifiedFilter, vk::SamplerMipmapMode::eLinear,
				adressingMode, adressingMode, adressingMode, .0f, maxAnistrophy > .0f, maxAnistrophy, vk::False,
				vk::CompareOp::eAlways, .0f, .0f, vk::BorderColor::eIntOpaqueBlack, vk::False);

		sampler = m_device.device.createSampler(samplerInfo);
	}

	void Texture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		vk::CommandBuffer commandBuffer = m_device.beginSingleTimeCommands();

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
