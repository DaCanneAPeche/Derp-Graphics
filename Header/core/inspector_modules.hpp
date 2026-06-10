#pragma once

#include "imgui.h"
#include "vulkan_renderer/vulkan_tool_box.hpp"
#include "entt/entt.hpp"
#include "core/system.hpp"
#include "core/debug_messages_storer.hpp"

namespace dg
{
  namespace inspector_modules
  {
    void time(float deltaTime);
    void vulkanInfos(dg::VulkanToolBox& vulkanToolBox, uint32_t pushConstantSize);
    void entities(entt::registry& registry);
    void systems();
    void logMessagesConsoleWindow(std::vector<DebugMessage>& messages);
  }
}
