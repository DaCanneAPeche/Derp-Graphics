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
          if (!isInputRespected(keys, mods, key, necessaryMods, forbiddenMods))
            return;

          signalHandler.send(signal);
          inputMap[signal] = true;
        });

    signalHandler.on(dg::config::Signals::KEY_RELEASE, [keys,
        signal, necessaryMods, forbiddenMods, this](Key key, KeyboardMods mods)
        {
          if (isInputRespected(keys, mods, key, necessaryMods, forbiddenMods))
            inputMap[signal] = false;
        });
  }

  void Scene::bindInput(Key key, dg::config::Signals signal,
      KeyboardMods necessaryMods, KeyboardMods forbiddenMods)
  {
    std::vector<Key> keys = {key};
    bindInput(keys, signal, necessaryMods, forbiddenMods);
  }
  
  bool Scene::isInputRespected(const std::vector<Key>& possibleKeys, KeyboardMods usedMods,
      Key pressedKey, KeyboardMods necessaryMods, KeyboardMods forbiddenMods)
  { 
    return // pressedKey not inside vector
        (std::find(possibleKeys.begin(), possibleKeys.end(), pressedKey) != possibleKeys.end()) && 
        ((necessaryMods & usedMods) == necessaryMods) && // check for necessary mods
        ((forbiddenMods & usedMods) == KeyboardMods::none); // check for forbidden mods
  }
}
