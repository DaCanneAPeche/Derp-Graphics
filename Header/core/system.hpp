#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>

#include "entt/entt.hpp"

#include "core/scene.hpp"

namespace dg
{
  class ISystem { };

  template <typename... Components>
  class System : public ISystem
  {
    public:

      virtual void onCreation(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onReplace(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onDestruct(Scene& scene, entt::entity entity, Components&...) {}
      virtual void update(Scene& scene, entt::entity entity, Components&...) {}

      void foo();

      bool active = true;

    private:

  };

  namespace _systems
  {
      inline std::vector<std::shared_ptr<ISystem>> allSystems;
      // static std::unordered_map<int, std::weak_ptr<System>> s_idMap;

      template <class T>
      int addSystem()
      {
        std::shared_ptr<ISystem> newSystem = std::make_shared<T>();
        _systems::allSystems.push_back(newSystem);

        return 0; // It needs to have a return in order to be called outside a function
      }

  } // _systems 

} // dg

#define DG_REGISTER_SYSTEM(X) static int _##X = dg::_systems::addSystem<X>();
