#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "entt/entt.hpp"

#include "core/scene.hpp"

namespace dg
{

  // TODO: change it so it is manually called from the system
  // Can work with this deduction but updating g++ is annoying
  template <class Base, class Derived>
    struct CheckIfFunctionsAreOverriden
    {
      using DerivedType = std::remove_reference_t<Derived>;

      bool onCreation = !std::is_same_v<decltype(&DerivedType::onCreation),
           decltype(&Base::onCreation)>;
      bool onReplace = !std::is_same_v<decltype(&DerivedType::onReplace),
           decltype(&Base::onReplace)>;
      bool onDestruct = !std::is_same_v<decltype(&DerivedType::onDestruct),
           decltype(&Base::onDestruct)>;
      // bool update = &Parent::update == &Derived::update;
    };

  struct ISystem
  {
    // Calls update function for all entity concerned
    virtual void IUpdate() {}
    virtual void init() {}

    entt::registry* pRegistry = nullptr;
    Scene* pScene = nullptr;
    bool active = true;
    std::string_view name = "Unknown";
  };

  template <class ...Components>
  struct GroupFactory
  {
    static auto create(entt::registry* pRegistry)
    {
      return pRegistry->group<Components...>();
    }
  };

  template <class Component>
  struct ViewFactory
  {
    static auto create(entt::registry* pRegistry)
    {
      return pRegistry->view<Component>();
    }
  };

  template <typename... Components>
  class System : public ISystem
  {
    using FirstComponent = std::tuple_element_t<0, std::tuple<Components...>>;

    // Specialization because views are better for one component and groups for
    // multiple components
    using Factory = typename std::conditional_t<
      sizeof...(Components) == 1,
      ViewFactory<FirstComponent>,
      GroupFactory<Components...>
        >;

    public:

      int id = 0;

      void IUpdate() override
      {
        if (!active) return;
        assert(pRegistry != nullptr && "pRegistry was not assigned");
        assert(pScene != nullptr && "pScene was not assigned");

        auto group = Factory::create(pRegistry);
        for (auto entity : group)
        {
          std::apply([entity, this](auto &&... args) {
            update(*pScene, entity, args...);
              }, group.get(entity));
        }
      }

      void init() override
      {
        setName();

        assert(pRegistry != nullptr && "pRegistry was not assigned");
        if constexpr(sizeof...(Components) == 0)
        {
          LOG_WARNING << "System without any components was created";
        }

        else if constexpr(sizeof...(Components) == 1)
        {
          // TODO: that does not work
          CheckIfFunctionsAreOverriden<System<Components...>, decltype(*this)>
            areFuncOverriden;

          // LOGD << areFuncOverriden.onCreation;
          // LOGD << areFuncOverriden.onReplace;

          if (areFuncOverriden.onCreation)
            pRegistry->on_construct<FirstComponent>()
              .template connect<&System<Components...>::_singleComponentOnCreation>(*this);

          if (areFuncOverriden.onReplace)
            pRegistry->on_construct<FirstComponent>()
              .template connect<&System<Components...>::_singleComponentOnReplace>(*this);

          if (areFuncOverriden.onDestruct)
            pRegistry->on_construct<FirstComponent>()
              .template connect<&System<Components...>::_singleComponentOnDestruct>(*this);
        }

        else (initSignals<Components>(), ...);
      }


      virtual void onCreation(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onReplace(Scene& scene, entt::entity entity, Components&...) {}
      virtual void onDestruct(Scene& scene, entt::entity entity, Components&...) {}
      virtual void update(Scene& scene, entt::entity entity, Components&...) {}

    protected:

    private:
      // template <class Self> => When g++ will be supported on debian
      // void setName(this Self&& self)
      void setName()
      {
        name = entt::type_name<std::remove_reference_t<decltype(*this)>()>().value();
      }

      template <class T>
      void initSignals()
      {
        CheckIfFunctionsAreOverriden<System<Components...>, decltype(*this)>
          areFuncOverriden;

        if (areFuncOverriden.onCreation)
          pRegistry->on_construct<T>()
            .template connect<&System<Components...>::_onCreation>(*this);

        if (areFuncOverriden.onReplace)
          pRegistry->on_construct<T>()
            .template connect<&System<Components...>::_onReplace>(*this);

        if (areFuncOverriden.onDestruct)
          pRegistry->on_construct<T>()
            .template connect<&System<Components...>::_onDestruct>(*this);
      }

      void _onCreation(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        if (pRegistry->all_of<Components...>(entity))
          std::apply([entity, this](auto &&... args) {
            onCreation(*pScene, entity, args...);
              }, pRegistry->get<Components...>(entity));
      }

      void _onReplace(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        if (pRegistry->all_of<Components...>(entity))
          std::apply([entity, this](auto &&... args) {
            onReplace(*pScene, entity, args...);
              }, pRegistry->get<Components...>(entity));
      }

      void _onDestruct(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        if (pRegistry->all_of<Components...>(entity))
          std::apply([entity, this](auto &&... args) {
            onDestruct(*pScene, entity, args...);
              }, pRegistry->get<Components...>(entity));
      }

      void _singleComponentOnCreation(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        onCreation(*pScene, entity, pRegistry->get<FirstComponent>(entity));
      }

      void _singleComponentOnReplace(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        onReplace(*pScene, entity, pRegistry->get<FirstComponent>(entity));
      }

      void _singleComponentOnDestruct(entt::registry&, entt::entity entity)
      {
        assert(pScene != nullptr && "pScene was not assigned");
        onDestruct(*pScene, entity, pRegistry->get<FirstComponent>(entity));
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

  template <class System>
  struct RegisterSystem
  {
    RegisterSystem()
    {
      _systems::addSystem<System>();
    }
  };

} // dg

