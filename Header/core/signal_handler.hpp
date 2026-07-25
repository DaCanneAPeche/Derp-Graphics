#pragma once

#include <unordered_map>
#include <functional>
#include <any>

#include <plog/Log.h>

#include "core/events.hpp"
#include "utils/castable_to.hpp"

namespace dg
{
  struct LifeCycle
  {
    bool alive = false;
  };

  template <class... CallbackArgsTypes>
  struct SignalCallback
  {
    std::function<void(CallbackArgsTypes...)> func;
    size_t lifeCycleIndex;
  };

  using SignalType = uint32_t;
  class SignalManager
  {
    public:

      static SignalManager* get()
      {
        if (!s_instance) s_instance = new SignalManager();

        return s_instance;
      }

      static void on(CastableTo<SignalType> auto signal, auto callback, size_t lifeCycleIndex)
      {
        get()->IOn(signal, callback, lifeCycleIndex);
      }

      template <class... CallbackArgsTypes>
      static void send(CastableTo<SignalType> auto signal, CallbackArgsTypes... arguments)
      {
        get()->ISend(signal, arguments...);
      }

      static void shutdown()
      {
        delete s_instance;
        s_instance = nullptr;
      }

      static bool isLifeCycleAlive(size_t lifeCycleIndex)
      {
        return get()->IIsLifeCycleAlive(lifeCycleIndex);
      }

      static size_t createLifeCycle()
      {
        return get()->ICreateLifeCycle();
      }

      static LifeCycle& getLifeCycle(size_t lifeCycleIndex)
      {
        return get()->IGetLifeCycle(lifeCycleIndex);
      }
      
      // Not true by default because really annoying with key ups and things
      static inline bool warnOnUselessSends = false;

      SignalManager(const SignalManager&) = delete;
      SignalManager& operator=(const SignalManager&) = delete;
  
    private:
      SignalManager() {}

      void IOn(CastableTo<SignalType> auto signal, auto callback, size_t lifeCycleIndex)
      {
        m_signalMap[static_cast<SignalType>(signal)].push_back({std::function(callback), lifeCycleIndex});
      }

      template <class... CallbackArgsTypes>
      void ISend(CastableTo<SignalType> auto signal, CallbackArgsTypes... arguments)
      {
        if (!m_signalMap.contains(static_cast<SignalType>(signal)))
        {
          LOG_WARNING_IF(warnOnUselessSends)
            << "Signal sent without existing callbacks : " << static_cast<SignalType>(signal);
          return;
        }

        for (const std::any& _callback : m_signalMap[static_cast<SignalType>(signal)])
        {
          auto callback = std::any_cast<SignalCallback<CallbackArgsTypes...>>(_callback);
          if (IIsLifeCycleAlive(callback.lifeCycle)) callback.func(arguments...);
        }
      }

      bool IIsLifeCycleAlive(size_t lifeCycleIndex)
      {
        return m_lifeCycles[lifeCycleIndex].alive;
      }

      size_t ICreateLifeCycle()
      {
        m_lifeCycles.emplace_back(true);
        return m_lifeCycles.size() - 1;
      }

      LifeCycle& IGetLifeCycle(size_t lifeCycleIndex)
      {
        return m_lifeCycles[lifeCycleIndex];
      }

      std::unordered_map<SignalType, std::vector<std::any>> m_signalMap;
      std::vector<LifeCycle> m_lifeCycles = {LifeCycle {true}};

      static inline SignalManager* s_instance = nullptr;
  };

}
