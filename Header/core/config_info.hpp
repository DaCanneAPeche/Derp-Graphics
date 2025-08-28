#pragma once

#include <string>
#include <array>

namespace dg
{
  struct ConfigInfo
  {
    struct
    {
      std::string name;
      std::array<uint8_t, 3> version;
    } application;

    struct
    {
      std::string name;
      std::array<uint16_t, 2> size; 
    } window;
  };
}
