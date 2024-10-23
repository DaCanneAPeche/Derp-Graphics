#pragma once

#include <string>
#include <array>

namespace dg
{

  struct ApplicationInfo {
    const std::string& name;
    std::array<uint8_t, 3> version;
  };

}
