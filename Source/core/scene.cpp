#include "core/scene.hpp"
#include "core/application.hpp"

namespace dg
{
  void Scene::init(Application *application)
  {
    app = application;
    assetManager = std::make_unique<AssetManager>(getAssets(), app->vulkanToolBox);
    assetManager->loadAll();
    app->renderer.updateTextures(*assetManager);
  }
}
