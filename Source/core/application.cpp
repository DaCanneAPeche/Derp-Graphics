#include "core/application.hpp"
#include "core/inputs.hpp"
#include "core/system.hpp"

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
    createRenderPass();
    renderer.init();

    setupSignalHandler();

    PLOG_INFO << "Init finished";

    PLOG_INFO << "Found " << _systems::allSystems.size() << " systems";

    initSystems();
  }

  Application::~Application()
  {
    renderer.clean();
  }

  void Application::changeScene(config::Scenes sceneId)
  {
    if (currentScene != nullptr) currentScene->end();
    currentScene = m_scenes[static_cast<uint32_t>(sceneId)]();
    currentScene->init(this);
    for (auto& system : _systems::allSystems) system->pScene = currentScene.get();
    currentScene->start();
  }

  void Application::run()
  {
    renderer.recreateSwapChain();
    changeScene(static_cast<config::Scenes>(0));
    while (!renderer.shouldWindowClose())
    {
      m_timer.start();

      renderer.pollEvents();

      update();

      currentScene->update();

      for (auto& system : _systems::allSystems) system->IUpdate();

      currentScene->lateUpdate();

      lateUpdate();

      Frame frame = renderer.startFrame();
      render(frame);

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();

      ImGui::NewFrame();
      imguiRender();
      ImGui::Render();

      frame.submit();

      deltaTime = m_timer.getElapsedTime().count() / 1000; // As seconds
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

      Key _key = Key::unknown;
      if (key != GLFW_KEY_UNKNOWN) _key = static_cast<Key>(key);

      currentScene->signalHandler.send(actionMap[action], _key,
          static_cast<KeyboardMods>(mods));
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

      currentScene->signalHandler.send(actionMap[action],
          static_cast<MouseButton>(button), static_cast<KeyboardMods>(mods));
    };

  }

  void Application::initSystems()
  {
    for (auto& system : _systems::allSystems)
    {
      system->pRegistry = &registry;
      system->init();
    }
  }

}
