#pragma once

#include "imgui.h"
#include "vulkan_renderer/vulkan_tool_box.hpp"
#include "vulkan_renderer/push_constant.hpp"
#include "entt/entt.hpp"
#include "core/system.hpp"

namespace dg
{
  namespace inspector_modules
  {
    void time(float deltaTime);
    void vulkanInfos(dg::VulkanToolBox& vulkanToolBox);
    void entities(entt::registry& registry);
    void systems();
  }
}
