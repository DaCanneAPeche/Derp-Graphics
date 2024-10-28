#pragma once

#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "core/transform2d.hpp"
#include "vulkan_renderer/push_constant.hpp"

#include <memory>
#include "vulkan/vulkan.hpp"

namespace dg
{

	class Sprite
	{
	public:
		Sprite(std::shared_ptr<Texture> _texture);

    Transform2d transform {};
    std::shared_ptr<Texture> texture;
    std::unique_ptr<Model> model;

    void draw(vk::CommandBuffer& commandBuffer,
        vk::PipelineLayout& pipelineLayout) const;
    [[nodiscard]] vk::DescriptorImageInfo getImageInfo() const;

	private:
	};
	
} /* dg */ 
