#include "core/application.hpp"

namespace dg
{
  Application::Application(const dg::WindowInfo& windowInfo,
      const dg::ApplicationInfo& appInfo) : renderer(windowInfo, vulkanToolBox)
  {
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::RollingFileAppender<plog::TxtFormatter> rollingFileAppender("log.txt",
        1000000, 5);
    plog::init(plog::verbose, &consoleAppender).addAppender(&rollingFileAppender);

    PLOG_INFO << "Program started : " << appInfo.name;

    vulkanToolBox.init(appInfo, renderer.window);
    renderer.init();
    renderer.externalRendering = [this](vk::CommandBuffer& commandBuffer)
    {
      render(commandBuffer);
    };

    setupSignalHandler();

    PLOG_INFO << "Init finished";
  }

  Application::~Application()
  {
    renderer.clean();
  }

  void Application::changeScene(config::Scenes sceneId)
  {
    if (currentScene != nullptr) currentScene->end();
    currentScene = m_scenes[static_cast<size_t>(sceneId)]();
    currentScene->application = this;
    currentScene->start();
  }

  void Application::run()
  {
    changeScene(static_cast<config::Scenes>(0));
    while (!renderer.shouldWindowClose())
    {
        renderer.pollEvents();

        currentScene->update();
        update();
        currentScene->lateUpdate();

        renderer.draw();
    }
    renderer.waitIdle();
  }

  void Application::setupSignalHandler()
  {
    
    renderer.window.keyInputCallback = [this](GLFWwindow* window, int key, int scancode,
        int action, int mods)
    {
      config::signalPackets::KeyInput data { key, scancode, mods };
      std::unordered_map<int, config::Signals> actionMap = 
      {
        {GLFW_PRESS, config::Signals::KEY_PRESS},
        {GLFW_REPEAT, config::Signals::KEY_REPEAT},
        {GLFW_RELEASE, config::Signals::KEY_RELEASE}
      };

      signalHandler.send(actionMap[action], &data);
    };

    renderer.window.mouseMoveCallback = [this](GLFWwindow* window, double xPos,
        double yPos)
    {
      glm::vec<2, double> mousePosition = {xPos, yPos};
      signalHandler.send(config::Signals::MOUSE_MOVE, &mousePosition);
    };

    renderer.window.mouseButtonCallback = [this](GLFWwindow* window, int button,
        int action, int mods)
    {
      config::signalPackets::MouseButton data { button, mods };
      std::unordered_map<int, config::Signals> actionMap = 
      {
        {GLFW_PRESS, config::Signals::MOUSE_PRESS},
        {GLFW_RELEASE, config::Signals::MOUSE_RELEASE}
      };

      signalHandler.send(actionMap[action], &data);
    };

  }
}
