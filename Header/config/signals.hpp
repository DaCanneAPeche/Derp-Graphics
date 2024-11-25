#pragma once

#include <stdint.h>

namespace dg
{
  namespace config
  {

    namespace signalPackets
    {
      struct KeyInput {
        int key, scancode, mods;
      };

      struct MouseButton {
        int button, scancode;
      };
    }

    enum class Signals : uint32_t
    {
      KEY_PRESS = 0,
      KEY_REPEAT,
      KEY_RELEASE,

      MOUSE_MOVE,
      MOUSE_PRESS,
      MOUSE_RELEASE
    };
  }
}
