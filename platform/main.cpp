#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES 

#include "game.hpp"

int main(void)
{
  dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
  dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

  Game game(windowInfo, appInfo);
  game.init();
  game.run();
}
