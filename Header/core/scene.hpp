#pragma once

#include <memory>

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
    protected:

    private:
  };
}
