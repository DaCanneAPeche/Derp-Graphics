#pragma once

#include <chrono>
#include <any>
#include <functional>

#include "config/signals.hpp"
#include "core/signal_handler.hpp"

namespace dg
{
  using timerUnit = std::chrono::milliseconds;

  class Timer
  {
    public:

      Timer() {};
      Timer(SignalHandler& signalHandler) { p_signalHandler = &signalHandler; }
      Timer(SignalHandler* pSignalHandler) { p_signalHandler = pSignalHandler; }

      void start();
      void update();
      bool hasStarted() { return m_hasStarted; }

      timerUnit getElapsedTime();

      template <class... Types>
      void setTimeout(timerUnit timeout, config::Signals signal,
          Types... args)
      {
        if (p_signalHandler == nullptr)
          throw std::runtime_error("Timer needs a signel handler to call timeout");

        m_timeout = timeout;
        m_callbackSignal = signal;
        m_callback = [this, signal, args...]() {
          p_signalHandler->send(signal, args...);
        };
      }

      std::chrono::time_point<std::chrono::steady_clock> startingTime;

    private:

      config::Signals m_callbackSignal {0};
      timerUnit m_timeout {0};
      std::function<void()> m_callback; // Used to send signal
      SignalHandler* p_signalHandler = nullptr;
      bool m_hasStarted;

  };
}
