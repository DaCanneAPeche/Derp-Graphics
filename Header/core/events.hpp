#pragma once

#include <stdint.h>

namespace dg
{
  enum class Events : uint32_t
  {
    KeyPress = 0,
    KeyRepeat,
    KeyRelease,

    MouseMove,
    MousePress,
    MouseRelease,
    MouseScroll,

  };
}
