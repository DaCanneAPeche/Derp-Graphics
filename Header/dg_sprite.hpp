#pragma once

#include "dg_model.hpp"
#include "dg_texture.hpp"
#include "dg_device.hpp"
#include "dg_transform2d.hpp"
#include "dg_push_constant.hpp"

#include <memory>
#include "vulkan/vulkan.hpp"

namespace dg
{

	class Sprite
	{
	public:
		Sprite(Device& device, std::shared_ptr<Texture> _texture);

    Transform2d transform {};
    std::shared_ptr<Texture> texture;
    std::unique_ptr<Model> model;

    void draw(vk::CommandBuffer& commandBuffer,
        vk::PipelineLayout& pipelineLayout) const;
    [[nodiscard]] vk::DescriptorImageInfo getImageInfo() const;

	private:
    Device& m_device;
	};
	
} /* dg */ 
