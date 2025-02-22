#pragma once

#include <unordered_map>
#include <functional>
#include <any>

#include "config/signals.hpp"
#include <plog/Log.h>

namespace dg
{
  class SignalHandler
  {
    public:
      void on(uint32_t signal, auto callback)
      {
        m_signalMap[signal].push_back(std::function(callback));
      }

      template <class... Types>
      void send(uint32_t signal, Types... args)
      {
        if (!m_signalMap.contains(signal))
        {
          LOG_WARNING_IF(warnOnUselessSends)
            << "Signal sent without existing callbacks : " << signal;
          return;
        }

        for (const std::any& callback : m_signalMap[signal])
        {
          auto func = std::any_cast<std::function<void(Types...)>>(callback);
          func(args...);
        }
      }

      void on(config::Signals signal, auto callback)
      {
        on(static_cast<uint32_t>(signal), callback);
      }

      template <class... Types>
      void send(config::Signals signal, Types... args)
      {
        send(static_cast<uint32_t>(signal), args...);
      }

      // Not true by default because really annoying with key ups and things
      bool warnOnUselessSends = false;

    private:
      std::unordered_map<uint32_t, std::vector<std::any>> m_signalMap;
  };

  /*
   * As a singleton
  class SignalHandler
  {
    public:

      static void clear()
      {
        getInstance().IClear();
      }

      static void on(uint32_t signal, auto callback)
      {
        getInstance().IOn(signal, callback);
      }

      template <class... Types>
      static void send(uint32_t signal, Types... args)
      {
        getInstance().ISend(signal, args...);
      }

      static SignalHandler& getInstance()
      {
        static SignalHandler instance;
        return instance;
      }

    private:

      void IClear()
      {
        m_signalMap.clear();
      }

      void IOn(uint32_t signal, auto callback)
      {
        m_signalMap[signal].push_back(std::function(callback));
      }

      template <class... Types>
      void ISend(uint32_t signal, Types... args)
      {
        if (!m_signalMap.contains(signal))
        {
          LOG_WARNING << "Signal sent without existing callbacks : " << signal;
          return;
        }

        for (const std::any& callback : m_signalMap[signal])
        {
          auto func = std::any_cast<std::function<void(Types...)>>(callback);
          func(args...);
        }

      }

      // Signal -> vector of function callbacks
      std::unordered_map<uint32_t, std::vector<std::any>> m_signalMap;
  };
  */
}
