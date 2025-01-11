#include "core/application.hpp"

namespace dg
{

  static bool doesImGuiRequestInputs()
  {
    auto& io = ImGui::GetIO();
    return io.WantCaptureMouse || io.WantCaptureKeyboard;
  }

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
    renderer.externalRendering = [this]() { render(); }; // embedding cause static func
    renderer.imguiRendering = [this]() { imguiRender(); };

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
    currentScene = m_scenes[static_cast<uint32_t>(sceneId)]();
    currentScene->application = this;
    currentScene->start();
  }

  void Application::run()
  {
    changeScene(static_cast<config::Scenes>(0));
    renderer.recreateSwapChain();
    while (!renderer.shouldWindowClose())
    {
      m_timer.start();

      renderer.pollEvents();

      currentScene->update();
      update();
      currentScene->lateUpdate();

      renderer.draw();

      deltaTime = m_timer.getElapsedTime();
    }
    renderer.waitIdle();
  }

  void Application::setupSignalHandler()
  {
    
    renderer.window.keyInputCallback = [this](GLFWwindow* window, int key, int scancode,
        int action, int mods)
    {
      if (doesImGuiRequestInputs()) return;

      std::unordered_map<int, config::Signals> actionMap = 
      {
        {GLFW_PRESS, config::Signals::KEY_PRESS},
        {GLFW_REPEAT, config::Signals::KEY_REPEAT},
        {GLFW_RELEASE, config::Signals::KEY_RELEASE}
      };

      currentScene->signalHandler.send(actionMap[action], key, scancode, mods);
    };

    renderer.window.mouseMoveCallback = [this](GLFWwindow* window, double xPos,
        double yPos)
    {
      if (doesImGuiRequestInputs()) return;

      glm::vec<2, double> mousePosition = {xPos, yPos};
      currentScene->signalHandler.send(config::Signals::MOUSE_MOVE, mousePosition);
    };

    renderer.window.mouseButtonCallback = [this](GLFWwindow* window, int button,
        int action, int mods)
    {
      if (doesImGuiRequestInputs()) return;

      std::unordered_map<int, config::Signals> actionMap = 
      {
        {GLFW_PRESS, config::Signals::MOUSE_PRESS},
        {GLFW_RELEASE, config::Signals::MOUSE_RELEASE}
      };

      currentScene->signalHandler.send(actionMap[action], button, mods);
    };

  }
}
