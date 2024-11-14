#pragma once

#include <unordered_map>
#include <functional>

namespace dg
{
  class SignalHandler
  {
    public:
      void on(size_t signal, std::function<void(void*)> callback);
      void send(size_t signal, void* data);

    private:
      std::unordered_map<size_t, std::function<void(void*)>> m_signalMap;

  };
}
