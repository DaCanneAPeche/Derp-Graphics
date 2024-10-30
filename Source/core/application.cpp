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

    vulkanToolBox.init(appInfo, renderer.window);
    renderer.init();

    PLOG_INFO << "Init finished";
  }

  Application::~Application()
  {
    renderer.clean();
  }

  void Application::changeScene(config::Scenes sceneId)
  {
    currentScene = m_scenes[static_cast<DG_CONFIG_SCENES_TYPE>(sceneId)]();
  }

  void Application::run()
  {
    while (!renderer.shouldWindowClose())
    {
        renderer.pollEvents();
        renderer.draw();
    }
    renderer.waitIdle();
  }
}
