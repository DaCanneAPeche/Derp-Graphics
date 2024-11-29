#pragma once

#include <memory>
#include "core/signal_handler.hpp"

namespace dg
{
  class Application;

  class Scene
  {
    public:
      virtual void start() {};
      virtual void update() {};
      virtual void lateUpdate() {};
      virtual void end() {};

      Application* application = nullptr; 
      SignalHandler signalHandler;
    protected:

    private:
  };
}
