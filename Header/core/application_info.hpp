#pragma once

#include <string>
#include <array>

struct ApplicationInfo {
  const std::string& name;
  std::array<uint8_t, 3> version;
};
