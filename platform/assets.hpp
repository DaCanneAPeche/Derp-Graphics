#pragma once

#include "vulkan_renderer/asset_manager.hpp"

struct Assets : public dg::AssetManager
{
  using dg::AssetManager::AssetManager;

  dg::TextureAsset leclerc = texture("./assets/textures/leclerc.jpg");
  dg::TextureAsset rick = texture("./assets/textures/nothing_suspicious.jpeg");
  dg::TextureAsset atlas = texture("./assets/textures/text_atlas_test.png");
};
