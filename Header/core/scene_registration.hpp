#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "core/scene.hpp"

namespace dg
{
  namespace _scenes
  {
    inline std::vector<std::function<std::unique_ptr<Scene>()>> sceneFabricators;

    template <class SceneType>
    void addScene(uint32_t sceneId)
    {
      static_assert(std::is_base_of<Scene, SceneType>(), "Scene not a child of dg::Scene");
      uint32_t id = static_cast<uint32_t>(sceneId);

      if (sceneFabricators.size() <= id + 1) sceneFabricators.resize(id + 1);

      sceneFabricators[id] =
      []() -> std::unique_ptr<Scene>
      {
        return std::make_unique<SceneType>();
      };
    }
  }

  template <class SceneType>
  struct RegisterScene
  {
    RegisterScene(uint32_t sceneId)
    {
        _scenes::addScene<SceneType>(sceneId);
    }
  };
}
