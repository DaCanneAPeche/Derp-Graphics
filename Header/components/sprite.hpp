#pragma once

#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "core/transform2d.hpp"
#include "vulkan_renderer/push_constant.hpp"

#include <memory>
#include "vulkan/vulkan.hpp"

namespace comp
{

	class Sprite
	{
	public:
    dg::Transform2d transform {};
    std::unique_ptr<dg::Model> model;

	private:
	};
	
} /* comp */ 
