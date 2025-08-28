#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES 

#include "game.hpp"

int main(void)
{
  Game game(dg::ConfigInfo::fromTOML("assets/derp-config.toml"));
  game.init();
  game.run();
}
