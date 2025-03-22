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
    // Calls update function for all entity concerned
    virtual void IUpdate() {}
    virtual void init() {}

    entt::registry* pRegistry = nullptr;
    Scene* pScene = nullptr;
    bool active = true;
  };

  template <typename... Components>
  class MultiComponentsSystem : public ISystem
  {
    public:

      void IUpdate() override
      {
        assert(pRegistry != nullptr && "pRegistry was not assigned");
        assert(pScene != nullptr && "pScene was not assigned");

        auto group = pRegistry->group<Components...>();
        for (auto entity : group)
        {
          std::apply([entity, this](auto &&... args) {
            update(*pScene, entity, args...);
              }, group.get(entity));
        }
      }

      void init() override
      {
        assert(pRegistry != nullptr && "pRegistry was not assigned");
        (initSignals<Components>(), ...);
      }

    protected:

      virtual void onCreation(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onReplace(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onDestruct(Scene& scene, entt::entity entity, Components&...) {}
      virtual void update(Scene& scene, entt::entity entity, Components&...) {}

    private:

      template <class T>
      void initSignals()
      {
        pRegistry->on_construct<T>()
          .template connect<&MultiComponentsSystem<Components...>::_onCreation>(*this);

        pRegistry->on_construct<T>()
          .template connect<&MultiComponentsSystem<Components...>::_onReplace>(*this);

        pRegistry->on_construct<T>()
          .template connect<&MultiComponentsSystem<Components...>::_onDestruct>(*this);
      }

      virtual void _onCreation(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        if (pRegistry->all_of<Components...>(entity))
          std::apply([entity, this](auto &&... args) {
            onCreation(*pScene, entity, args...);
              }, pRegistry->get<Components...>());
      }

      virtual void _onReplace(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        if (pRegistry->all_of<Components...>(entity))
          std::apply([entity, this](auto &&... args) {
            onReplace(*pScene, entity, args...);
              }, pRegistry->get<Components...>());
      }

      virtual void _onDestruct(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        if (pRegistry->all_of<Components...>(entity))
          std::apply([entity, this](auto &&... args) {
            onDestruct(*pScene, entity, args...);
              }, pRegistry->get<Components...>());
      }

  };

  template <typename Component>
  class SingleComponentSystem : public ISystem
  {
    public:

      void IUpdate() override
      {
        assert(pRegistry != nullptr && "pRegistry was not assigned");
        assert(pScene != nullptr && "pScene was not assigned");

        auto view = pRegistry->view<Component>();
        for (auto entity : view)
        {
          auto [component] = view.get(entity);
          update(*pScene, entity, component);
        }
      }

      void init() override
      {
        assert(pRegistry != nullptr && "pRegistry was not assigned");

        pRegistry->on_construct<Component>()
          .template connect<&SingleComponentSystem<Component>::_onCreation>(*this);

        pRegistry->on_construct<Component>()
          .template connect<&SingleComponentSystem<Component>::_onReplace>(*this);

        pRegistry->on_construct<Component>()
          .template connect<&SingleComponentSystem<Component>::_onDestruct>(*this);
      }

    protected:

      virtual void onCreation(Scene& scene, entt::entity entity, Component&) {}
      virtual void onReplace(Scene& scene, entt::entity entity, Component&) {}
      virtual void onDestruct(Scene& scene, entt::entity entity, Component&) {}
      virtual void update(Scene& scene, entt::entity entity, Component&) {}

    private: 

      virtual void _onCreation(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        onCreation(*pScene, entity, pRegistry->get<Component>(entity));
      }

      virtual void _onReplace(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        onReplace(*pScene, entity, pRegistry->get<Component>(entity));
      }

      virtual void _onDestruct(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        onDestruct(*pScene, entity, pRegistry->get<Component>(entity));
      }
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

#define DG_REGISTER_SYSTEM(X) static int8_t _##X = dg::_systems::addSystem<X>();
