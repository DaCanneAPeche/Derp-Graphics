#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "vulkan_renderer/renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "core/application.hpp"

class MainScene : public dg::Scene
{

};

class Game : public dg::Application
{
  public:
    Game(const dg::WindowInfo& windowInfo, const dg::ApplicationInfo& appInfo)
      : dg::Application(windowInfo, appInfo)
    {
      addScene<MainScene>(dg::config::Scenes::MAIN);
    }

    ~Game()
    {

    }
};

int main(void)
{
  dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
  dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};
  
  Game game(windowInfo, appInfo);
  game.run();
}
