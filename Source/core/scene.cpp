#include "core/scene.hpp"
#include "core/application.hpp"

#include <algorithm>

namespace dg
{
  void Scene::init(Application *application)
  {
    app = application;
    assetManager = std::make_unique<AssetManager>(getAssets(), app->vulkanToolBox);
    assetManager->loadAll();
    app->renderer.updateTextures(*assetManager);
  }

  void Scene::bindInput(std::vector<Key>& keys, dg::config::Signals signal,
      KeyboardMods necessaryMods, KeyboardMods forbiddenMods)
  {
    assert((necessaryMods & forbiddenMods) == KeyboardMods::none &&
        "Necessary and forbidden mods cannot overlap !");

    signalHandler.on(dg::config::Signals::KEY_PRESS, [keys,
        signal, necessaryMods, forbiddenMods, this](Key key, KeyboardMods mods)
        {
          if ((std::find(keys.begin(), keys.end(), key) == keys.end()) || // key not inside vector
              ((necessaryMods & mods) != necessaryMods) || // check for necessary mods
              ((forbiddenMods & mods) != KeyboardMods::none)) return; // check for forbidden mods

          signalHandler.send(signal);
        });
  }

  void Scene::bindInput(Key key, dg::config::Signals signal,
      KeyboardMods necessaryMods, KeyboardMods forbiddenMods)
  {
    std::vector<Key> keys = {key};
    bindInput(keys, signal, necessaryMods, forbiddenMods);
  }
}
