#include "core/application.hpp"
#include "core/inputs.hpp"
#include "core/system.hpp"
#include "core/scene_registration.hpp"

namespace dg
{

  static bool doesImGuiRequestInputs()
  {
    auto& io = ImGui::GetIO();
    return io.WantCaptureMouse || io.WantCaptureKeyboard;
  }

  Application::Application(const WindowInfo& windowInfo,
      const ApplicationInfo& appInfo) : renderer(windowInfo, vulkanToolBox),
      m_appInfo(appInfo)
  {
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::RollingFileAppender<plog::TxtFormatter> rollingFileAppender("log.txt",
        1000000, 5);
    plog::init(plog::verbose, &consoleAppender).addAppender(&rollingFileAppender);

    PLOG_INFO << "Program started : " << appInfo.name;
    vulkanToolBox.init(m_appInfo, renderer.window);
  }

  Application::~Application()
  {
    renderer.clean();
  }

  void Application::changeScene(uint32_t sceneId)
  {
    if (currentScene != nullptr) currentScene->end();
    currentScene = _scenes::sceneFabricators[sceneId]();
    currentScene->init(this);
    for (auto& system : _systems::allSystems) system->pScene = currentScene.get();
    currentScene->start();
  }

  void Application::init()
  {
    renderer.init();

    setupSignalHandler();

    PLOG_INFO << "Init finished";

    PLOG_INFO << "Found " << _systems::allSystems.size() << " systems";

    initSystems();
    renderer.recreateSwapChain();
    changeScene(0);
    wasInitialized = true;
  }

  void Application::run()
  {
    assert(wasInitialized == true && "You need to call app.init() before app.run()");

    start();
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
