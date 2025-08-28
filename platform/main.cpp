#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES 

#include "game.hpp"

#include "core/config_info.hpp"
#include "rfl/toml.hpp"
#include "rfl.hpp"

int main(void)
{
  rfl::Result<dg::ConfigInfo> config = rfl::toml::load<dg::ConfigInfo>("assets/derp-config.toml");

  Game game(config.value());
  game.init();
  game.run();
}
