#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "core/scene.hpp"
#include "utils/castable_to.hpp"

namespace dg
{
  namespace _scenes
  {
    inline std::vector<std::function<std::unique_ptr<Scene>()>> sceneFabricators;

    template <std::derived_from<Scene> SceneType, CastableTo<uint32_t> IdType>
    void addScene(IdType sceneId)
    {
      uint32_t id = static_cast<uint32_t>(sceneId);

      if (sceneFabricators.size() <= id + 1) sceneFabricators.resize(id + 1);

      sceneFabricators[id] =
      []() -> std::unique_ptr<Scene>
      {
        return std::make_unique<SceneType>();
      };
    }
  }

  template <std::derived_from<Scene> SceneType>
  struct RegisterScene
  {
    template <CastableTo<uint32_t> IdType>
    RegisterScene(IdType sceneId)
    {
        _scenes::addScene<SceneType>(sceneId);
    }
  };
}
