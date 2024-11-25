#pragma once

#include <unordered_map>
#include <functional>

#include "config/signals.hpp"

namespace dg
{
  class SignalHandler
  {
    public:
      void on(size_t signal, std::function<void(void*)> callback);
      void send(size_t signal, void* data);

      void on(config::Signals signal, std::function<void(void*)> callback)
      {
        on(static_cast<size_t>(signal), callback);
      }
      void send(config::Signals signal, void* data)
      {
        send(static_cast<size_t>(signal), data);
      }

    private:
      std::unordered_map<size_t, std::function<void(void*)>> m_signalMap;

  };
}
