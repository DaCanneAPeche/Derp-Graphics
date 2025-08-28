#pragma once

#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "vulkan_renderer/frame.hpp"
#include "core/transform2d.hpp"
#include "vulkan_renderer/push_constant.hpp"
#include "core/inspector.hpp"

#include <memory>
#include "vulkan/vulkan.hpp"

#include "entt/entt.hpp"
#include "imgui.h"
#include <glm/gtc/constants.hpp>

namespace comp
{
  using namespace entt::literals;

	class Sprite
	{
	public:
    dg::Transform2d transform {};
    std::unique_ptr<dg::Model> model;
    uint32_t textureId = 0;

    dg::Inspector<Sprite> inspector;

    void draw(dg::Frame& frame)
    {
      dg::PushConstant push {
        .transform = transform.getMatrix(),
          .offset = transform.translation,
          .textureId = textureId
      };

      frame.pushConstant(push);

      model->bind(frame.commandBuffer);
      model->draw(frame.commandBuffer);
    }

	private:
	};
	
} /* comp */ 
