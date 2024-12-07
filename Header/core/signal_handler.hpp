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
      template <typename F>
      void on(uint32_t signal, F callback)
      {
        m_signalMap[signal] = std::function(callback);
      }

      template <class... Types>
      void send(uint32_t signal, Types... args)
      {
        if (!m_signalMap.contains(signal)) return;

        auto func =
          std::any_cast<std::function<void(Types...)>>(m_signalMap[signal]);
        func(args...);
      }

      template <typename F>
      void on(config::Signals signal, F callback)
      {
        on(static_cast<uint32_t>(signal), callback);
      }

      template <class... Types>
      void send(config::Signals signal, Types... args)
      {
        send(static_cast<uint32_t>(signal), args...);
      }

    private:
      std::unordered_map<uint32_t, std::any> m_signalMap;

  };
}
