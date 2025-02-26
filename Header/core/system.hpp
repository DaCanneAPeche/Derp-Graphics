#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>

#include "entt/entt.hpp"

#include "core/scene.hpp"

namespace dg
{
  struct ISystem
  {
    virtual void IUpdate(Scene& scene) {}
    entt::registry* pRegistry = nullptr;
  };

  template <typename... Components>
  class System : public ISystem
  {
    public:

      virtual void onCreation(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onReplace(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onDestruct(Scene& scene, entt::entity entity, Components&...) {}
      virtual void update(Scene& scene, entt::entity entity, Components&...) {}

      void IUpdate(Scene& scene) override
      {
        assert(pRegistry != nullptr && "pRegistry was not assigned");

        auto view = pRegistry->view<Components...>();
        for (auto entity : view)
        {
          std::apply([&scene, entity, this](auto &&... args) {
            update(scene, entity, args...);
              }, view.get(entity));
        }
      }

      void foo();

      bool active = true;

    protected:

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
