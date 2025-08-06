#pragma once

#include <functional>
#include <any>
#include <memory>
#include <vector>

#include "entt/entt.hpp"

#include "config/scenes.hpp"
#include "core/scene.hpp"
#include "core/application_info.hpp"
#include "core/timer.hpp"
#include "vulkan_renderer/renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/frame.hpp"

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

      void init(); // An init function meant to be called after the child constructor
      void run();

      dg::VulkanToolBox vulkanToolBox;
      dg::Renderer renderer;
      entt::registry registry;
      float deltaTime;

    protected:
      void changeScene(uint32_t sceneId);

      std::unique_ptr<Scene> currentScene;
      ApplicationInfo m_appInfo;

      virtual void start() {}; // Called after the renderer is initialized
      virtual void update() {};
      virtual void lateUpdate() {};
      virtual void render(Frame& frame) {};
      virtual void imguiRender() {};

    private:
      void setupSignalHandler();
      void initSystems();

      Timer m_timer;
      bool wasInitialized = false;
  };
}
