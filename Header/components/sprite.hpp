#pragma once

#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "vulkan_renderer/frame.hpp"
#include "core/transform2d.hpp"
#include "vulkan_renderer/push_constant.hpp"

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
    float ratio;

    Sprite()
    {
      entt::meta<Sprite>().func<&Sprite::inspect>("Inspector"_hs);
    }

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

    static void inspect(void* instance)
    {
      Sprite* self = static_cast<Sprite*>(instance);
      
      ImGui::Text("Sprite component");

      ImGui::SliderFloat2("Position", reinterpret_cast<float*>(&self->transform.translation), -1, 1);
      ImGui::SliderFloat("Rotation", &self->transform.rotation, 0, 2 * glm::pi<float>());
      ImGui::SliderFloat2("Scale", reinterpret_cast<float*>(&self->transform.scaling), -10, 10);
      ImGui::Text("Texture id : %i", self->textureId);

      ImGui::Separator();
    }

	private:
	};
	
} /* comp */ 
