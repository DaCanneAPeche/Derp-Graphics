#pragma once

#include "entt/entt.hpp"
#include "imgui.h"

namespace comp
{
  using namespace entt::literals;

  struct Position
  {
    float x = 0, y = 0;

    Position()
    {
      // entt::meta<Position>().func<&Position::inspect>("Inspector"_hs);
    }

    static void draw()
    {

    }
  };
}
