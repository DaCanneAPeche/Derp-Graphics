#pragma once

#include <string>
#include <array>

namespace dg
{

  struct ApplicationInfo {
    std::string name;
    std::array<uint8_t, 3> version;
  };

}
