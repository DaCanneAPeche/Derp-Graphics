#pragma once

#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "vulkan_renderer/renderer.hpp"
#include "core/transform2d.hpp"
#include "vulkan_renderer/push_constant.hpp"

#include <memory>
#include "vulkan/vulkan.hpp"

#include "entt/entt.hpp"
#include "imgui.h"

namespace comp
{
  using namespace entt::literals;

	class Sprite
	{
	public:
    dg::Transform2d transform {};
    std::unique_ptr<dg::Model> model;
    uint32_t textureId = 0;

    Sprite()
    {
      entt::meta<Sprite>().func<&Sprite::inspect>("Inspector"_hs);
    }

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

    static void inspect(void* instance)
    {
      Sprite* self = static_cast<Sprite*>(instance);
      
      ImGui::SliderFloat2("Position", reinterpret_cast<float*>(&self->transform.translation), -1, 1);
    }

	private:
	};
	
} /* comp */ 
