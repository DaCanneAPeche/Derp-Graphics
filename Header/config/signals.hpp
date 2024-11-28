#pragma once

#include <stdint.h>

namespace dg
{
  namespace config
  {

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
