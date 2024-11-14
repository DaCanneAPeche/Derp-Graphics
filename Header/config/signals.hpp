#pragma once

#include <stdint.h>

enum class Signals : uint32_t
{
  KEY_PRESS = 0,
  KEY_REPEAT,
  KEY_RELEASE,

  CURSOR_MOVE,
  MOUSE_PRESS,
  MOUSE_RELEASE
};
