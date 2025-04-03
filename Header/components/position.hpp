#pragma once

#include "core/inspector.hpp"

namespace comp
{
  struct Position
  {
    float x = 0, y = 0;
    int z = 10;

    dg::Inspector<Position> inspector;
  };
}
