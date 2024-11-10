#pragma once

#include <functional>
#include <any>
#include <memory>
#include <vector>

#include "entt/entt.hpp"

#include "config/scenes.hpp"
#include "core/scene.hpp"
#include "core/application_info.hpp"
#include "vulkan_renderer/renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

#include <plog/Log.h>
#include "plog/Init.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Appenders/RollingFileAppender.h"

namespace dg
{
  class Application
  {
    public:
      Application(const dg::WindowInfo& windowInfo,
          const dg::ApplicationInfo& appInfo);
      ~Application();

      void run();

      dg::VulkanToolBox vulkanToolBox;
      dg::Renderer renderer;
      entt::registry registry;

    protected:
      template <class T>
      void addScene(config::Scenes sceneId)
      {
        static_assert(std::is_base_of<Scene, T>(), "Scene not a child of dg::Scene");
        size_t id = static_cast<size_t>(sceneId);

        if (m_scenes.size() <= id + 1)
          m_scenes.resize(id + 1);

        m_scenes[id] =
        []() -> std::unique_ptr<Scene>
        {
          return std::make_unique<T>();
        };
      }

      void changeScene(config::Scenes sceneId);

      std::unique_ptr<Scene> currentScene;

      virtual void update() {};
    private:

      std::vector<std::function<std::unique_ptr<Scene>()>> m_scenes;
  };
}
