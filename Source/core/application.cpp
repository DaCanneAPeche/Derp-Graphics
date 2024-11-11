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
}
