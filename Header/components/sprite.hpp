#pragma once

#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "vulkan_renderer/renderer.hpp"
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
    uint32_t textureId = 0;

    void draw(dg::Renderer& renderer)
    {
      dg::PushConstant push {
        .transform = transform.getMatrix(),
          .offset = transform.translation,
          .textureId = textureId};

      renderer.pushConstant(push);

      model->bind(*renderer.pCurrentCommandBuffer);
      model->draw(*renderer.pCurrentCommandBuffer);
    }

	private:
	};
	
} /* comp */ 
